#ifndef __ADC_H_
#define __ADC_H_

#include <stdint.h>

int riotee_adc_init();
int riotee_adc_read(int16_t *dst, unsigned int analog_input);

#endif