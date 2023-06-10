#include "nrf.h"
#include "nrf_gpio.h"

#include "riotee.h"
#include "riotee_thresholds.h"

/* Having NFC pins in different state may increase leakage (see PS v1.5 6.13.3). PIN_THRCTL_H0 is NFC1, so we need to
 * synchronize pin PIN_NFC2.
 */
#define PIN_NFC2 10

typedef enum {
  LOW,
  Z,
  HIGH,
} gpio_state_t;

static int gpio_set(uint32_t pin, gpio_state_t gpio_state) {
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

  if (gpio_state == Z) {
    reg->PIN_CNF[pin] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
  } else {
    if (gpio_state == LOW)
      reg->OUTCLR = (1 << pin);
    else
      reg->OUTSET = (1 << pin);

    reg->PIN_CNF[pin] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
  }
  return 0;
}

int thresholds_low_set(thr_low_t thr) {
  gpio_state_t gpio_states[2];
  switch (thr) {
    case 0:
      gpio_states[0] = LOW;
      gpio_states[1] = LOW;
      break;
    case 2:
      gpio_states[0] = HIGH;
      gpio_states[1] = LOW;
      break;
    case 6:
      gpio_states[0] = LOW;
      gpio_states[1] = HIGH;
      break;
    case 8:
      gpio_states[0] = HIGH;
      gpio_states[1] = HIGH;
      break;
    default:
      return -1;
  }
  gpio_set(PIN_THRCTL_L0, gpio_states[0]);
  gpio_set(PIN_THRCTL_L1, gpio_states[1]);

  return 0;
}
int thresholds_high_set(thr_high_t thr) {
  gpio_state_t gpio_states[2];
  switch (thr) {
    case 0:
      gpio_states[0] = LOW;
      gpio_states[1] = LOW;
      break;
    case 1:
      gpio_states[0] = Z;
      gpio_states[1] = LOW;
      break;
    case 2:
      gpio_states[0] = HIGH;
      gpio_states[1] = LOW;
      break;
    case 3:
      gpio_states[0] = LOW;
      gpio_states[1] = Z;
      break;
    case 4:
      gpio_states[0] = Z;
      gpio_states[1] = Z;
      break;
    case 5:
      gpio_states[0] = HIGH;
      gpio_states[1] = Z;
      break;
    case 6:
      gpio_states[0] = LOW;
      gpio_states[1] = HIGH;
      break;
    case 7:
      gpio_states[0] = Z;
      gpio_states[1] = HIGH;
      break;
    case 8:
      gpio_states[0] = HIGH;
      gpio_states[1] = HIGH;
      break;
    default:
      return -1;
  }
  gpio_set(PIN_THRCTL_H0, gpio_states[0]);
  /* Synchronize other NFC pin, see PS v1.5 6.13.3 */
  gpio_set(PIN_NFC2, gpio_states[0]);

  gpio_set(PIN_THRCTL_H1, gpio_states[1]);

  return 0;
}
