# ADC

The nRF52 on the Riotee Module has a 12-bit successive approximation ADC.

# Reference
::: doxy.riotee.Function
  name: void riotee_adc_init(void)

::: doxy.riotee.Function
  name: int16_t riotee_adc_read(riotee_adc_input_t in)

::: doxy.riotee.Function
  name: int riotee_adc_sample(int16_t *dst, riotee_adc_cfg_t *cfg)
