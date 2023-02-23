#include <stdlib.h>
#include "nrf.h"
#include "nrf_gpio.h"

#include "gpint.h"

static GPINT_CALLBACK registry[32] = {0};

void GPIOTE_IRQHandler(void) {
  if (NRF_GPIOTE->EVENTS_PORT == 1) {
    for (uint32_t i = 0; i < 32; i++) {
      GPINT_CALLBACK cb = registry[i];
      NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&i);
      if ((cb != NULL) && (reg->LATCH & (1 << i))) {
        gpint_unregister(i);
        reg->LATCH |= (1 << i);
        NRF_GPIOTE->EVENTS_PORT = 0;
        cb(i);
        return;
      }
    }
  }
}

int gpint_init(void) {
  NRF_P0->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT;
  NRF_P1->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT;
  NRF_GPIOTE->EVENTS_PORT = 0;

  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
  NVIC_EnableIRQ(GPIOTE_IRQn);
  NVIC_SetPriority(GPIOTE_IRQn, 1);
  return 0;
}

int gpint_register(uint32_t pin, gpint_level_t level, GPINT_CALLBACK cb) {
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

  if (pin > 31)
    return -1;

  if (registry[pin] != NULL)
    return -2;
  registry[pin] = cb;

  reg->LATCH |= (1 << pin);

  /* Order is important, see nrf52833 errata 210*/
  if (level == GPINT_LEVEL_HIGH) {
    reg->PIN_CNF[pin] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
    reg->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
  } else {
    reg->PIN_CNF[pin] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
    reg->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
  }
  return 0;
}

int gpint_unregister(uint32_t pin) {
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

  reg->PIN_CNF[pin] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                      (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

  registry[pin] = NULL;
  return 0;
}