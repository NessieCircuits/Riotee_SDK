#include <stdlib.h>
#include "nrf.h"
#include "riotee_gpio.h"

#include "FreeRTOS.h"
#include "runtime.h"
#include "gpint.h"
#include "riotee_gpio.h"

static GPINT_CALLBACK registry[42] = {0};

void GPIOTE_IRQHandler(void) {
  if (NRF_GPIOTE->EVENTS_PORT == 1) {
    for (uint32_t i = 0; i < 42; i++) {
      GPINT_CALLBACK cb = registry[i];
      riotee_gpio_port_t *reg = riotee_gpio_get_port(i);
      int pin_idx = riotee_gpio_get_pin_idx(i);
      if ((cb != NULL) && (reg->LATCH & (1 << pin_idx))) {
        gpint_unregister(i);
        reg->LATCH |= (1 << pin_idx);
        NRF_GPIOTE->EVENTS_PORT = 0;
        cb(i);
        return;
      }
    }
  }
}

int gpint_register(unsigned pin, riotee_gpio_level_t level, riotee_gpio_in_pull_t pull, GPINT_CALLBACK cb) {
  riotee_gpio_port_t *reg = riotee_gpio_get_port(pin);
  int pin_idx = riotee_gpio_get_pin_idx(pin);

  if (pin > 41)
    return RIOTEE_GPIO_ERR_UNSUPPORTED;

  if (registry[pin] != NULL)
    return RIOTEE_GPIO_ERR_BUSY;
  registry[pin] = cb;

  reg->PIN_CNF[pin_idx] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (pull << GPIO_PIN_CNF_PULL_Pos);
  reg->LATCH |= (1 << pin_idx);

  /* Order is important, see nrf52833 errata 210*/
  if (level == RIOTEE_GPIO_LEVEL_HIGH) {
    reg->PIN_CNF[pin_idx] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
  } else {
    reg->PIN_CNF[pin_idx] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
  }
  return RIOTEE_GPIO_ERR_OK;
}

int gpint_unregister(unsigned int pin) {
  if (registry[pin] == NULL)
    return RIOTEE_GPIO_ERR_GENERIC;

  riotee_gpio_port_t *reg = riotee_gpio_get_port(pin);
  int pin_idx = riotee_gpio_get_pin_idx(pin);

  reg->PIN_CNF[pin_idx] &= ~GPIO_PIN_CNF_SENSE_Msk;

  registry[pin] = NULL;
  return RIOTEE_GPIO_ERR_OK;
}

void riotee_gpio_init(void) {
  NRF_P0->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT;
  NRF_P1->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT;
  NRF_GPIOTE->EVENTS_PORT = 0;

  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
  NVIC_EnableIRQ(GPIOTE_IRQn);
  NVIC_SetPriority(GPIOTE_IRQn, 1);
}

static void wait_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_GPINT, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int riotee_gpio_wait_level(unsigned int pin, riotee_gpio_level_t level, riotee_gpio_in_pull_t pull) {
  unsigned long notification_value;
  taskENTER_CRITICAL();
  xTaskNotifyStateClearIndexed(usr_task_handle, 1);
  gpint_register(pin, level, pull, wait_callback);
  taskEXIT_CRITICAL();
  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  if (notification_value != EVT_GPINT)
    return -1;
  return 0;
}
