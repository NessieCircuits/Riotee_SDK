#ifndef __ROPTEE_ADC_H_
#define __RIOTEE_ADC_H_

#include <stdint.h>
#include "riotee.h"

typedef enum {
  RIOTEE_ADC_INPUT_NC = 0,
  RIOTEE_ADC_INPUT_VCAP = 6,
  RIOTEE_ADC_INPUT_A0 = 3,
  RIOTEE_ADC_INPUT_A1 = 4,
} riotee_adc_input_t;

typedef enum {
  RIOTEE_ADC_RESOLUTION_8BIT = 0UL,   ///< 8 bit resolution.
  RIOTEE_ADC_RESOLUTION_10BIT = 1UL,  ///< 10 bit resolution.
  RIOTEE_ADC_RESOLUTION_12BIT = 2UL,  ///< 12 bit resolution.
  RIOTEE_ADC_RESOLUTION_14BIT = 3UL   ///< 14 bit resolution.
} riotee_adc_resolution_t;

typedef enum {
  RIOTEE_ADC_OVERSAMPLE_DISABLED = 0UL,  ///< No oversampling.
  RIOTEE_ADC_OVERSAMPLE_2X = 1UL,        ///< Oversample 2x.
  RIOTEE_ADC_OVERSAMPLE_4X = 2UL,        ///< Oversample 4x.
  RIOTEE_ADC_OVERSAMPLE_8X = 3UL,        ///< Oversample 8x.
  RIOTEE_ADC_OVERSAMPLE_16X = 4UL,       ///< Oversample 16x.
  RIOTEE_ADC_OVERSAMPLE_32X = 5UL,       ///< Oversample 32x.
  RIOTEE_ADC_OVERSAMPLE_64X = 6UL,       ///< Oversample 64x.
  RIOTEE_ADC_OVERSAMPLE_128X = 7UL,      ///< Oversample 128x.
  RIOTEE_ADC_OVERSAMPLE_256X = 8UL       ///< Oversample 256x.
} riotee_adc_oversample_t;

typedef enum {
  RIOTEE_ADC_GAIN1_6 = 0UL,  ///< Gain factor 1/6.
  RIOTEE_ADC_GAIN1_5 = 1UL,  ///< Gain factor 1/5.
  RIOTEE_ADC_GAIN1_4 = 2UL,  ///< Gain factor 1/4.
  RIOTEE_ADC_GAIN1_3 = 3UL,  ///< Gain factor 1/3.
  RIOTEE_ADC_GAIN1_2 = 4UL,  ///< Gain factor 1/2.
  RIOTEE_ADC_GAIN1 = 5UL,    ///< Gain factor 1.
  RIOTEE_ADC_GAIN2 = 6UL,    ///< Gain factor 2.
  RIOTEE_ADC_GAIN4 = 7UL,    ///< Gain factor 4.
} riotee_adc_gain_t;

typedef enum {
  RIOTEE_ADC_REFERENCE_INTERNAL = 0UL,  ///< Internal reference (0.6 V).
  RIOTEE_ADC_REFERENCE_VDD4 = 1UL       ///< VDD/4 as reference.
} riotee_adc_reference_t;

typedef enum {
  RIOTEE_ADC_ACQTIME_3US = 0UL,
  RIOTEE_ADC_ACQTIME_5US = 1UL,
  RIOTEE_ADC_ACQTIME_10US = 2UL,
  RIOTEE_ADC_ACQTIME_15US = 3UL,
  RIOTEE_ADC_ACQTIME_20US = 4UL,
  RIOTEE_ADC_ACQTIME_40US = 5UL
} riotee_adc_acqtime_t;

typedef struct {
  riotee_adc_gain_t gain;            ///< Gain control value.
  riotee_adc_reference_t reference;  ///< Reference control value.
  riotee_adc_acqtime_t acq_time;     ///< Acquisition time.
  riotee_adc_input_t input_pos;
  riotee_adc_input_t input_neg;
  riotee_adc_oversample_t oversampling;
  unsigned int n_samples;
  unsigned int sample_interval_ticks32;
} riotee_adc_cfg_t;

int riotee_adc_init(void);

/* Samples ADC into provided buffer. */
int riotee_adc_sample(int16_t *dst, riotee_adc_cfg_t *cfg);

/* Reads a single ADC sample. */
static inline int riotee_adc_read(float *dst, riotee_adc_input_t input) {
  int16_t adc_res;
  int rc;
  riotee_adc_cfg_t cfg = {.gain = RIOTEE_ADC_GAIN1_4,
                          .reference = RIOTEE_ADC_REFERENCE_VDD4,
                          .acq_time = RIOTEE_ADC_ACQTIME_5US,
                          .input_pos = input,
                          .input_neg = RIOTEE_ADC_INPUT_NC,
                          .oversampling = RIOTEE_ADC_OVERSAMPLE_DISABLED,
                          .n_samples = 1};

  if ((rc = riotee_adc_sample(&adc_res, &cfg)) != 0)
    return rc;
  *dst = ((float)adc_res) / 4096.0f * 2.0f;
  return 0;
}

/* Converts binary ADC result to voltage. */
float riotee_adc_adc2vadc(int16_t adc, riotee_adc_cfg_t *cfg);

/* Converts ADC voltage to capacitor voltage based on amplifier gain. */
static inline float riotee_adc_vadc2vcap(float v_adc) {
  /* A capacitor voltage of 4.8V produces 1.727V on the ADC input */
  return v_adc / 1.727f * 4.8f;
}

static inline int riotee_adc_pin2input(riotee_adc_input_t *input, unsigned int pin) {
  switch (pin) {
    case PIN_D2:
      *input = RIOTEE_ADC_INPUT_A0;
      return 0;
    case PIN_D3:
      *input = RIOTEE_ADC_INPUT_A1;
      return 0;
    default:
      return -1;
  }
}

#endif /* __RIOTEE_ADC_H_ */