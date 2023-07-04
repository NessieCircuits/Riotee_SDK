#include <limits.h>
#include "riotee_adc.h"

int analogRead(uint32_t pin) {
    riotee_adc_input_t in;
    if(riotee_adc_pin2input(&in, pin)!=0)
        return INT_MIN;

    return (int) riotee_adc_read(in);
}