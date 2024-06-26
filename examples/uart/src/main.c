#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_uart.h"
#include "printf.h"

/* This gets called after every reset */
void lateinit(void) {
  riotee_uart_init(PIN_D1, RIOTEE_UART_BAUDRATE_1000000);
  printf("Hello Riotee!\r\n");
}

int main(void) {
  int counter = 0;
  for (;;) {
    printf("Loop: %d\r\n", counter++);
    riotee_sleep_ms(1000);
  }
}