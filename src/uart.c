#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "riotee_uart.h"

int uart_init(uint32_t pseltxd) {
  NRF_UART0->PSEL.TXD = pseltxd;
  NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1M;
  nrf_gpio_cfg_input(pseltxd, NRF_GPIO_PIN_PULLUP);
  return 0;
}

void _putchar(char character) {
  taskENTER_CRITICAL();
  NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
  NRF_UART0->TXD = character;
  NRF_UART0->TASKS_STARTTX = 1UL;
  while (NRF_UART0->EVENTS_TXDRDY == 0) {
  };
  NRF_UART0->EVENTS_TXDRDY = 0;
  NRF_UART0->TASKS_STOPTX = 1UL;
  NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled;
  taskEXIT_CRITICAL();
}