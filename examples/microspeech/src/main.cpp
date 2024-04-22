#include "string.h"

#include "tensorflow/lite/core/c/common.h"
#include "micro_model_settings.h"

#include "printf.h"
#include "riotee.h"
#include "riotee_adc.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_stella.h"
#include "riotee_thresholds.h"

#include "vm1010.h"
#include "shtc3.h"

/* Pin D10 enables/disables microphone on the Riotee Sensor Shield (low active)
 */
#define PIN_MICROPHONE_DISABLE PIN_D5
#define N_SAMPLES 16000

void startup_callback(void) {
  /* Call this early to put SHTC3 into low power mode */
  shtc3_init();
  /* Disable microphone to reduce current consumption. */
  riotee_gpio_cfg_output(PIN_MICROPHONE_DISABLE);
  riotee_gpio_set(PIN_MICROPHONE_DISABLE);
}

void reset_callback(void) {
  riotee_thresholds_low_set(THR_LOW_3V1);

  vm1010_cfg_t cfg = {.pin_mode = PIN_D10, .pin_dout = PIN_D4, .pin_vout = PIN_D2, .pin_vbias = PIN_D3};
  vm1010_init(&cfg);
}

void suspend_callback(void) {
  /* Disable the microphone */
  riotee_gpio_set(PIN_MICROPHONE_DISABLE);
  vm1010_exit();
}

int16_t samples[N_SAMPLES];

/* Remove mean and scale audio to fit int16 range */
void prescale_audio(int16_t *dst, const size_t n_samples) {
  int val_min = dst[0];
  int val_max = dst[0];
  int val_sum = 0;

  for (unsigned int i = 0; i < n_samples; i++) {
    if (dst[i] < val_min)
      val_min = dst[i];
    if (dst[i] > val_max)
      val_max = dst[i];
    val_sum += dst[i];
  }
  int val_mean = val_sum / (int)n_samples;
  int max_trans = val_max - val_mean;
  int min_trans = val_mean - val_min;
  int scale = max_trans;

  if (min_trans > scale)
    scale = min_trans;

  for (unsigned int i = 0; i < n_samples; i++) {
    int tmp = (((int)dst[i]) - val_mean) * 32767;

    dst[i] = (int16_t)(tmp / scale);
  }
}

int main(void) {
  int rc;
  struct ClassificationResult result;
  printf("main..\r\n");

  for (;;) {
    riotee_wait_cap_charged();

    /* Enable microphone */
    riotee_gpio_clear(PIN_MICROPHONE_DISABLE);
    /* Wait for 2ms for V_BIAS to come up */
    riotee_sleep_ticks(70);
    printf("Activating wake-on-sound..\r\n");

    /* Wait for wake-on-sound signal from microphone */
    if ((rc = vm1010_wait4sound()) != RIOTEE_SUCCESS) {
      printf("Error while waiting for sound: %d", rc);
    }
    /* Wait until microphone can be sampled.. */
    riotee_sleep_ms(2);

    /* Sample with 16384Hz instead of the 16000 expected by the model */
    rc = vm1010_sample(samples, N_SAMPLES, 2);
    /* Disable the microphone */
    riotee_gpio_set(PIN_MICROPHONE_DISABLE);

    if (rc != RIOTEE_SUCCESS) {
      printf("Sampling failed with %d\r\n", rc);
      continue;
    }

    printf("Sampling done. Start processing..\r\n");
    prescale_audio(samples, N_SAMPLES);

    if ((rc = Classify(result, samples, N_SAMPLES)) == kTfLiteOk)
      printf("Heard %s with %.2f probability\r\n", kCategoryLabels[result.category_idx], result.probability);
    else
      printf("Classification failed with error %d\r\n", rc);
  }
}
