/**
 * @defgroup adc ADC driver
 * @{
 */

#ifndef __RIOTEE_ADC_H_
#define __RIOTEE_ADC_H_

#include <stdint.h>
#include "riotee.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  /** Input not connected. */
  RIOTEE_ADC_INPUT_NC = 0,
  /** Capacitor voltage. */
  RIOTEE_ADC_INPUT_VCAP = 6,
  /** Analog input A0 */
  RIOTEE_ADC_INPUT_A0 = 3,
  /** Analog input A1 */
  RIOTEE_ADC_INPUT_A1 = 4,
} riotee_adc_input_t;

typedef enum {
  /** 8 bit resolution */
  RIOTEE_ADC_RESOLUTION_8BIT = 0UL,
  /** 10 bit resolution */
  RIOTEE_ADC_RESOLUTION_10BIT = 1UL,
  /** 12 bit resolution */
  RIOTEE_ADC_RESOLUTION_12BIT = 2UL,
  /** 14 bit resolution */
  RIOTEE_ADC_RESOLUTION_14BIT = 3UL
} riotee_adc_resolution_t;

typedef enum {
  /** No oversampling. */
  RIOTEE_ADC_OVERSAMPLE_DISABLED = 0UL,
  /** Oversample 2x. */
  RIOTEE_ADC_OVERSAMPLE_2X = 1UL,
  /** Oversample 4x. */
  RIOTEE_ADC_OVERSAMPLE_4X = 2UL,
  /** Oversample 8x. */
  RIOTEE_ADC_OVERSAMPLE_8X = 3UL,
  /** Oversample 16x. */
  RIOTEE_ADC_OVERSAMPLE_16X = 4UL,
  /** Oversample 32x. */
  RIOTEE_ADC_OVERSAMPLE_32X = 5UL,
  /** Oversample 64x. */
  RIOTEE_ADC_OVERSAMPLE_64X = 6UL,
  /** Oversample 128x. */
  RIOTEE_ADC_OVERSAMPLE_128X = 7UL,
  /** Oversample 256x. */
  RIOTEE_ADC_OVERSAMPLE_256X = 8UL
} riotee_adc_oversample_t;

typedef enum {
  /** Gain factor 1/6. */
  RIOTEE_ADC_GAIN1_6 = 0UL,
  /** Gain factor 1/5. */
  RIOTEE_ADC_GAIN1_5 = 1UL,
  /** Gain factor 1/4. */
  RIOTEE_ADC_GAIN1_4 = 2UL,
  /** Gain factor 1/3. */
  RIOTEE_ADC_GAIN1_3 = 3UL,
  /** Gain factor 1/2. */
  RIOTEE_ADC_GAIN1_2 = 4UL,
  /** Gain factor 1. */
  RIOTEE_ADC_GAIN1 = 5UL,
  /** Gain factor 2. */
  RIOTEE_ADC_GAIN2 = 6UL,
  /** Gain factor 4. */
  RIOTEE_ADC_GAIN4 = 7UL,
} riotee_adc_gain_t;

typedef enum {
  /** Internal reference (0.6 V). */
  RIOTEE_ADC_REFERENCE_INTERNAL = 0UL,
  /** VDD/4 reference. */
  RIOTEE_ADC_REFERENCE_VDD4 = 1UL
} riotee_adc_reference_t;

typedef enum {
  /** 3us acquisition time. */
  RIOTEE_ADC_ACQTIME_3US = 0UL,
  /** 5us acquisition time. */
  RIOTEE_ADC_ACQTIME_5US = 1UL,
  /** 10us acquisition time. */
  RIOTEE_ADC_ACQTIME_10US = 2UL,
  /** 15us acquisition time. */
  RIOTEE_ADC_ACQTIME_15US = 3UL,
  /** 20us acquisition time. */
  RIOTEE_ADC_ACQTIME_20US = 4UL,
  /** 40us acquisition time. */
  RIOTEE_ADC_ACQTIME_40US = 5UL
} riotee_adc_acqtime_t;

typedef enum {
  /** No pullup on ADC input. */
  RIOTEE_ADC_RES_NOPULL = 0UL,
  /** Pulldown on ADC input. */
  RIOTEE_ADC_RES_PULLDOWN = 1UL,
  /** Pullup on ADC input. */
  RIOTEE_ADC_RES_PULLUP = 2UL,
  /** Pullup and pulldown (VDD/2) on ADC input. */
  RIOTEE_ADC_RES_VDD1_2 = 3UL,
} riotee_adc_res_t;

typedef struct {
  /** Gain of ADC pre-amplifier. */
  riotee_adc_gain_t gain;
  /** ADC reference. */
  riotee_adc_reference_t reference;
  /** Acquisition time. */
  riotee_adc_acqtime_t acq_time;
  /** ADC positive input. */
  riotee_adc_input_t input_pos;
  /** ADC negative input. */
  riotee_adc_input_t input_neg;
  /** Positive input resistor config. */
  riotee_adc_res_t res_pos;
  /** Negative input resistor config. */
  riotee_adc_res_t res_neg;
  /** Oversampling factor. */
  riotee_adc_oversample_t oversampling;
  /** Number of samples to be taken. */
  unsigned int n_samples;
  /**  Sample interval in ticks on a 32kHz clock. */
  unsigned int sample_interval_ticks32;
} riotee_adc_cfg_t;

/**
 * @brief Initializes ADC. Must be called once after reset before ADC can be used.
 *
 */
void riotee_adc_init(void);

/**
 * @brief Reads multiple samples from the ADC.
 *
 * Periodically samples the ADC with a specified sampling interval storing the samples in the provided buffer. Blocks
 * until all samples are taken or until the operation is aborted due to low energy.
 *
 * @param dst Buffer where samples are stored.
 * @param cfg ADC and sampling configuration.
 *
 * @retval RIOTEE_SUCCESS       Sampling completed.
 * @retval RIOTEE_ERR_RESET    Reset occured while sampling.
 * @retval RIOTEE_ERR_TEARDOWN Teardown occured while sampling.
 */
riotee_rc_t riotee_adc_sample(int16_t *dst, riotee_adc_cfg_t *cfg);

/**
 * @brief Reads a sample from the ADC.
 *
 * Reads one sample from the ADC with a preconfigured configuration. Blocks until sample is taken. Gain and reference
 * settings are chosen such that the full 12-bit input range equals the supply voltage.
 *
 * @param in Analog input.
 * @return int16_t ADC sample as 12-bit value w.r.t. the supply voltage.
 */
int16_t riotee_adc_read(riotee_adc_input_t in);

/**
 * @brief Converts a raw binary value sampled from the ADC to a voltage value.
 *
 * @param adc 12-bit ADC sample.
 * @param cfg ADC configuration that was used for taking the sample.
 * @return float Voltage value.
 */
float riotee_adc_adc2vadc(int16_t adc, riotee_adc_cfg_t *cfg);

/**
 * @brief Converts ADC input voltage to capacitor voltage based on amplifier gain.
 *
 * @param v_adc ADC input voltage.
 * @return float Capacitor voltage.
 */
static inline float riotee_adc_vadc2vcap(float v_adc) {
  /* A capacitor voltage of 4.8V produces 1.727V on the ADC input */
  return v_adc / 1.727f * 4.8f;
}

/**
 * @brief Translates digital pin to analog input channel.
 *
 * @param input Pointer where ADC input gets stored.
 * @param pin Pin number.
 *
 * @retval RIOTEE_SUCCESS         Successfully converted.
 * @retval RIOTEE_ERR_INVALIDARG Pin is not an ADC input.
 */
static inline riotee_rc_t riotee_adc_pin2input(riotee_adc_input_t *input, unsigned int pin) {
  switch (pin) {
    case PIN_D2:
      *input = RIOTEE_ADC_INPUT_A0;
      return RIOTEE_SUCCESS;
    case PIN_D3:
      *input = RIOTEE_ADC_INPUT_A1;
      return RIOTEE_SUCCESS;
    default:
      return RIOTEE_ERR_INVALIDARG;
  }
}

#ifdef __cplusplus
}
#endif

#endif /** @} __RIOTEE_ADC_H_ */
