#include <stdlib.h>
#include "nrf.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "runtime.h"
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

int gpint_register(uint32_t pin, gpint_level_t level, gpint_pin_pull_t pull, GPINT_CALLBACK cb) {
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

  if (pin > 31)
    return GPINT_ERR_UNSUPPORTED;

  if (registry[pin] != NULL)
    return GPINT_ERR_BUSY;
  registry[pin] = cb;

  reg->PIN_CNF[pin] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (pull << GPIO_PIN_CNF_PULL_Pos);
  reg->LATCH |= (1 << pin);

  /* Order is important, see nrf52833 errata 210*/
  if (level == GPINT_LEVEL_HIGH) {
    reg->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
  } else {
    reg->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
  }
  return GPINT_ERR_OK;
}

int gpint_unregister(uint32_t pin) {
  if (registry[pin] == NULL)
    return GPINT_ERR_GENERIC;

  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

  reg->PIN_CNF[pin] &= ~GPIO_PIN_CNF_SENSE_Msk;

  registry[pin] = NULL;
  return GPINT_ERR_OK;
}

static void wait_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyIndexedFromISR(usr_task_handle, 1, USR_EVT_GPINT, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int gpint_wait(uint32_t pin, gpint_level_t level, gpint_pin_pull_t pull) {
  unsigned long notification_value;
  taskENTER_CRITICAL();
  xTaskNotifyStateClear(usr_task_handle);
  gpint_register(pin, level, pull, wait_callback);
  taskEXIT_CRITICAL();
  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  if (notification_value != USR_EVT_GPINT)
    return -1;
  return 0;
}
