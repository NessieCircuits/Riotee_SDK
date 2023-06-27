
#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_uart.h"
#include "riotee_adc.h"
#include "printf.h"

#include "arm_math.h"
#include "arm_const_structs.h"

static float32_t samples[1024];
static float32_t fft_res[1024];

static arm_rfft_fast_instance_f32 fft_inst;

/* This gets called after every reset */
void reset_callback(void) {
  riotee_uart_init(PIN_D1, 1000000);
  riotee_adc_init();
}

int16_t samples_raw[1024];

static void adc2float(float32_t *dst, int16_t *src, size_t n) {
  float max = 0;
  float sum = 0;

  for (unsigned int i = 0; i < n; i++) {
    dst[i] = ((float)dst[i]) / 4096.0f * 2.0f;
    if (fabs(dst[i]) > max)
      max = fabs(dst[i]);
    sum += dst[i];
  }
  float mean = sum / n / max;

  for (unsigned int i = 0; i < n; i++) {
    dst[i] = dst[i] / max - mean;
  }
}

int main(void) {
  riotee_adc_cfg_t adc_cfg = {.acq_time = RIOTEE_ADC_ACQTIME_5US,
                              .gain = RIOTEE_ADC_GAIN1_4,
                              .reference = RIOTEE_ADC_REFERENCE_VDD4,
                              .input_neg = RIOTEE_ADC_INPUT_NC,
                              .input_pos = RIOTEE_ADC_INPUT_A0,
                              .oversampling = RIOTEE_ADC_OVERSAMPLE_DISABLED,
                              .n_samples = 1024,
                              .sample_interval_ticks32 = 8};

  arm_rfft_fast_init_f32(&fft_inst, 1024);

  for (;;) {
    riotee_wait_cap_charged();
    printf("Sampling");
    riotee_adc_sample(samples_raw, &adc_cfg);
    printf("Converting");
    adc2float(samples, samples_raw, 1024);
    printf("FFT");
    arm_rfft_fast_f32(&fft_inst, samples, fft_res, 0);
    printf("Done");
  }
}
