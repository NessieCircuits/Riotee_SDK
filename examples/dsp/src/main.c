
#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_uart.h"
#include "riotee_adc.h"
#include "printf.h"

#include "arm_math.h"
#include "arm_const_structs.h"

/* This gets called after every reset */
void lateinit(void) {
  riotee_uart_init(PIN_D1, 1000000);
  riotee_adc_init();
}

#define FFT_SIZE (1024)
static int16_t samples_i16[FFT_SIZE];
static float32_t samples_f32[FFT_SIZE];
static float32_t fft_result[FFT_SIZE];

static arm_rfft_fast_instance_f32 fft_inst;

static void adc2float(float32_t *dst, int16_t *src, size_t n) {
  /* outputs normed float [-1;+1] */
  float max = fabs(src[0]);
  float sum = 0;

  for (unsigned int i = 1; i < n; i++) {
    if (fabs(src[i]) > max)
      max = fabs(src[i]);
    sum += src[i];
  }
  float mean = sum / n;

  for (unsigned int i = 0; i < n; i++) {
    dst[i] = (src[i] - mean) / max;
  }
}

int main(void) {
  riotee_adc_cfg_t adc_cfg = {.acq_time = RIOTEE_ADC_ACQTIME_5US,
                              .gain = RIOTEE_ADC_GAIN1_4,
                              .reference = RIOTEE_ADC_REFERENCE_VDD4,
                              .input_neg = RIOTEE_ADC_INPUT_NC,
                              .input_pos = RIOTEE_ADC_INPUT_A0,
                              .oversampling = RIOTEE_ADC_OVERSAMPLE_DISABLED,
                              .n_samples = FFT_SIZE,
                              .sample_interval_ticks32 = 8};

  arm_rfft_fast_init_f32(&fft_inst, FFT_SIZE);

  for (;;) {
    riotee_wait_cap_charged();
    printf("Sample, ");
    riotee_adc_sample(samples_i16, &adc_cfg);
    printf("Convert, ");
    adc2float(samples_f32, samples_i16, FFT_SIZE);
    printf("FFT, ");
    arm_rfft_fast_f32(&fft_inst, samples_f32, fft_result, 0);
    printf("Done\r\n");
  }
}
