#include "riotee_gpio.h"
#include "led.hpp"

LED::LED(unsigned int pin) : _pin(pin) {
  riotee_gpio_cfg_output(_pin);
};

void LED::on(void) {
  riotee_gpio_set(_pin);
}

void LED::off(void) {
  riotee_gpio_clear(_pin);
}