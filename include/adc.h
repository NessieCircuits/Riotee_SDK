#ifndef __ADC_H_
#define __ADC_H_

#include <stdint.h>

int adc_init();
int adc_read(int16_t *dst, unsigned int analog_input);

#endif