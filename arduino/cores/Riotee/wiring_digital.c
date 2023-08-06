#include "riotee.h"
#include "riotee_gpio.h"

#include "Arduino.h"
#include "external/ArduinoCore-API/api/Common.h"

void pinMode(pin_size_t pinNumber, PinMode pinMode) {
  switch (pinMode) {
    case INPUT:
      riotee_gpio_cfg_input(pinNumber, RIOTEE_GPIO_IN_NOPULL);
      break;

    case INPUT_PULLUP:
      riotee_gpio_cfg_input(pinNumber, RIOTEE_GPIO_IN_PULLUP);

      break;

    case INPUT_PULLDOWN:
      riotee_gpio_cfg_input(pinNumber, RIOTEE_GPIO_IN_PULLDOWN);

      break;

    case OUTPUT:
      riotee_gpio_cfg_output(pinNumber);

      break;

    default:
      break;
  }
}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {
  switch (status) {
    case LOW:
      riotee_gpio_clear(pinNumber);
      break;

    default:
      riotee_gpio_set(pinNumber);
      break;
  }

  return;
}