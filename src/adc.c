#include "nrf.h"
#include "nrf_saadc.h"
#include "riotee.h"
#include "FreeRTOS.h"
#include "task.h"
#include "runtime.h"

/* This points to the task currently blocking on GPINT event */
static TaskHandle_t waiting_task;

uint16_t adc_res;

void SAADC_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_SAADC->EVENTS_END == 1) {
    NRF_SAADC->EVENTS_END = 0;
    xTaskNotifyIndexedFromISR(waiting_task, 1, USR_EVT_ADC, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int adc_init() {
  NRF_SAADC->OVERSAMPLE = NRF_SAADC_OVERSAMPLE_DISABLED;
  NRF_SAADC->RESOLUTION = NRF_SAADC_RESOLUTION_12BIT;

  NRF_SAADC->CH[0].CONFIG =
      ((NRF_SAADC_RESISTOR_DISABLED << SAADC_CH_CONFIG_RESP_Pos) & SAADC_CH_CONFIG_RESP_Msk) |
      ((NRF_SAADC_RESISTOR_DISABLED << SAADC_CH_CONFIG_RESN_Pos) & SAADC_CH_CONFIG_RESN_Msk) |
      ((NRF_SAADC_GAIN1_3 << SAADC_CH_CONFIG_GAIN_Pos) & SAADC_CH_CONFIG_GAIN_Msk) |
      ((NRF_SAADC_REFERENCE_INTERNAL << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk) |
      ((NRF_SAADC_ACQTIME_5US << SAADC_CH_CONFIG_TACQ_Pos) & SAADC_CH_CONFIG_TACQ_Msk) |
      ((NRF_SAADC_MODE_SINGLE_ENDED << SAADC_CH_CONFIG_MODE_Pos) & SAADC_CH_CONFIG_MODE_Msk) |
      ((NRF_SAADC_BURST_DISABLED << SAADC_CH_CONFIG_BURST_Pos) & SAADC_CH_CONFIG_BURST_Msk);

  NRF_SAADC->RESULT.MAXCNT = 1;

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);

  NRF_PPI->CH[4].EEP = (uint32_t)&NRF_SAADC->EVENTS_STARTED;
  NRF_PPI->CH[4].TEP = (uint32_t)&NRF_SAADC->TASKS_SAMPLE;
  NRF_PPI->CHENSET = PPI_CHENSET_CH4_Msk;

  NRF_PPI->CH[5].EEP = (uint32_t)&NRF_SAADC->EVENTS_RESULTDONE;
  NRF_PPI->CH[5].TEP = (uint32_t)&NRF_SAADC->TASKS_STOP;
  NRF_PPI->CHENSET = PPI_CHENSET_CH5_Msk;

  NRF_SAADC->INTENSET = SAADC_INTENSET_END_Msk;
  NVIC_EnableIRQ(SAADC_IRQn);

  return 0;
}

int adc_read(int16_t *dst, unsigned int analog_input) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  waiting_task = xTaskGetCurrentTaskHandle();
  /* PSELP value is AIN number plus one */
  NRF_SAADC->CH[0].PSELP = analog_input + 1;
  NRF_SAADC->RESULT.MAXCNT = 1;
  NRF_SAADC->RESULT.PTR = (uint32_t)dst;
  xTaskNotifyStateClearIndexed(waiting_task, 1);
  NRF_SAADC->TASKS_START = 1;
  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  if (notification_value == USR_EVT_ADC)
    return 0;
  else
    return -1;
}