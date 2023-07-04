#include "Serial.h"
#include "riotee.h"
#include "riotee_uart.h"

#include <stdint.h>

int SerialClass::available() {
  return -1;
}
int SerialClass::read() {
  return -1;
}
int SerialClass::peek() {
  return -1;
}
void SerialClass::flush() {
  return;
}

void SerialClass::begin(unsigned long baudrate) {
  riotee_uart_baudrate_t riotee_baudrate;

  if (baudrate <= 1200) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_1200;
  } else if (baudrate <= 2400) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_2400;
  } else if (baudrate <= 4800) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_4800;
  } else if (baudrate <= 9600) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_9600;
  } else if (baudrate <= 14400) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_14400;
  } else if (baudrate <= 19200) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_19200;
  } else if (baudrate <= 28800) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_28800;
  } else if (baudrate <= 38400) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_38400;
  } else if (baudrate <= 57600) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_57600;
  } else if (baudrate <= 76800) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_76800;
  } else if (baudrate <= 115200) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_115200;
  } else if (baudrate <= 230400) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_230400;
  } else if (baudrate <= 250000) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_250000;
  } else if (baudrate <= 460800) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_460800;
  } else if (baudrate <= 921600) {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_921600;
  } else {
    riotee_baudrate = RIOTEE_UART_BAUDRATE_1000000;
  }
  riotee_uart_init(PIN_D1, riotee_baudrate);
}

size_t SerialClass::write(const uint8_t *buffer, size_t size) {
  for (unsigned int i = 0; i < size; i++)
    riotee_putc(*buffer++);
  return size;
}

size_t SerialClass::write(uint8_t c) {
  riotee_putc(c);
  return 1;
}

SerialClass Serial;