#include <limits.h>
#include "riotee_adc.h"

#include "external/ArduinoCore-API/api/Common.h"

int analogRead(pin_size_t pinNumber) {
  riotee_adc_input_t in;
  if (riotee_adc_pin2input(&in, pinNumber) != 0)
    return INT_MIN;

  return (int)riotee_adc_read(in);
}