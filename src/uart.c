#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "riotee_uart.h"

int riotee_uart_init(uint32_t pseltxd, uint32_t baudrate) {
  NRF_UART0->PSEL.TXD = pseltxd;
  riotee_uart_set_baudrate(baudrate);
  nrf_gpio_cfg_input(pseltxd, NRF_GPIO_PIN_PULLUP);
  return 0;
}

int riotee_uart_set_baudrate(uint32_t baudrate) {
  switch (baudrate) {
    case 1200:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1200;
      return 0;
    case 2400:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud2400;
      return 0;
    case 4800:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud4800;
      return 0;
    case 9600:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud9600;
      return 0;
    case 14400:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud14400;
      return 0;
    case 19200:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud19200;
      return 0;
    case 28800:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud28800;
      return 0;
    case 31250:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud31250;
      return 0;
    case 38400:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud38400;
      return 0;
    case 56000:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud56000;
      return 0;
    case 57600:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud57600;
      return 0;
    case 76800:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud76800;
      return 0;
    case 115200:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
      return 0;
    case 230400:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud230400;
      return 0;
    case 250000:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud250000;
      return 0;
    case 460800:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud460800;
      return 0;
    case 921600:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud921600;
      return 0;
    case 1000000:
      NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1M;
      return 0;
    default:
      return -1;
  }
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