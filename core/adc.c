#include "nrf.h"
#include "riotee_adc.h"
#include "riotee.h"
#include "FreeRTOS.h"
#include "task.h"
#include "runtime.h"
#include "nrf_gpio.h"

TEARDOWN_FUN(adc_teardown_ptr);

/* Number of samples that still need to be taken before buffer is filled. */
static unsigned int samples_remaining;
static unsigned int sample_interval_ticks32;

/* Inverse gain lookup table, indexed by riotee_adc_gain_t */
static const float gain_lut[] = {6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 1.0f / 2, 1.0f / 4};
/* Reference lookup table, indexed by enum riotee_adc_reference_t */
static const float ref_lut[] = {0.6f, 0.5f};

float riotee_adc_adc2vadc(int16_t adc, riotee_adc_cfg_t *cfg) {
  float res_fac;
  /* See nRF52833 Product Specification v1.5 sec 6.21.3*/
  if (cfg->input_neg != RIOTEE_ADC_INPUT_NC) {
    res_fac = 1.0f / (1 << 11);
  } else {
    res_fac = 1.0f / (1 << 12);
  }
  return ref_lut[cfg->reference] * ((float)adc) * gain_lut[cfg->gain] * res_fac;
}

static inline void stop_sampling(void) {
  NRF_SAADC->INTENCLR = SAADC_INTENCLR_END_Msk;

  NRF_RTC0->EVTENCLR = RTC_EVTEN_COMPARE2_Msk;
  NRF_SAADC->TASKS_STOP = 1;
  NRF_SAADC->EVENTS_END = 0;

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);
  adc_teardown_ptr = NULL;
}

void SAADC_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  NRF_SAADC->EVENTS_END = 0;

  if (--samples_remaining == 0) {
    stop_sampling();
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_ADC, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  } else {
    NRF_SAADC->RESULT.PTR += 2;
    NRF_RTC0->CC[2] = (NRF_RTC0->COUNTER + sample_interval_ticks32) % (1 << 24);
  }
}

static void teardown(void) {
  stop_sampling();
  xTaskNotifyIndexed(usr_task_handle, 1, EVT_TEARDOWN, eSetValueWithOverwrite);
}

int riotee_adc_init(void) {
  NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;

  NRF_PPI->CH[3].EEP = (uint32_t)&NRF_RTC0->EVENTS_COMPARE[2];
  NRF_PPI->CH[3].TEP = (uint32_t)&NRF_SAADC->TASKS_START;
  NRF_PPI->CHENSET = PPI_CHENSET_CH3_Msk;

  NRF_PPI->CH[4].EEP = (uint32_t)&NRF_SAADC->EVENTS_STARTED;
  NRF_PPI->CH[4].TEP = (uint32_t)&NRF_SAADC->TASKS_SAMPLE;
  NRF_PPI->CHENSET = PPI_CHENSET_CH4_Msk;

  NRF_PPI->CH[5].EEP = (uint32_t)&NRF_SAADC->EVENTS_END;
  NRF_PPI->CH[5].TEP = (uint32_t)&NRF_SAADC->TASKS_STOP;
  NRF_PPI->CHENSET = PPI_CHENSET_CH5_Msk;

  NVIC_EnableIRQ(SAADC_IRQn);

  return 0;
}

int riotee_adc_sample(int16_t *dst, riotee_adc_cfg_t *cfg) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);

  NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos) |
                            (SAADC_CH_CONFIG_RESN_Bypass << SAADC_CH_CONFIG_RESN_Pos) |
                            ((cfg->gain << SAADC_CH_CONFIG_GAIN_Pos) & SAADC_CH_CONFIG_GAIN_Msk) |
                            ((cfg->reference << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk) |
                            ((cfg->acq_time << SAADC_CH_CONFIG_TACQ_Pos) & SAADC_CH_CONFIG_TACQ_Msk);

  /* If oversampling is enabled, take samples as fast as possible in burst mode */
  NRF_SAADC->OVERSAMPLE = cfg->oversampling;
  if (cfg->oversampling != RIOTEE_ADC_OVERSAMPLE_DISABLED)
    NRF_SAADC->CH[0].CONFIG |= (SAADC_CH_CONFIG_BURST_Enabled << SAADC_CH_CONFIG_BURST_Pos);

  NRF_SAADC->CH[0].PSELP = cfg->input_pos;
  NRF_SAADC->CH[0].PSELN = cfg->input_neg;
  if (cfg->input_neg != RIOTEE_ADC_INPUT_NC)
    NRF_SAADC->CH[0].CONFIG |= (SAADC_CH_CONFIG_MODE_Diff << SAADC_CH_CONFIG_MODE_Pos);

  /* Take only one sample, manage buffer in software */
  NRF_SAADC->RESULT.PTR = (uint32_t)dst;
  NRF_SAADC->RESULT.MAXCNT = 1;
  samples_remaining = cfg->n_samples;

  xTaskNotifyStateClearIndexed(usr_task_handle, 1);

  NRF_SAADC->INTENSET = SAADC_INTENSET_END_Msk;

  /* Register teardown function so runtime can abort us */
  adc_teardown_ptr = teardown;

  if (cfg->n_samples > 1) {
    sample_interval_ticks32 = cfg->sample_interval_ticks32;
    NRF_RTC0->CC[2] = (NRF_RTC0->COUNTER + cfg->sample_interval_ticks32) % (1 << 24);
    NRF_RTC0->EVTENSET = RTC_EVTEN_COMPARE2_Msk;
  }
  NRF_SAADC->TASKS_START = 1;

  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  if (notification_value == EVT_ADC)
    return 0;
  else
    return -1;
}
