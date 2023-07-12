
#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_uart.h"
#include "printf.h"
#include "riotee_gpio.h"

#include "shtc3.h"
#include "vm1010.h"

#define PIN_MICROPHONE_DISABLE PIN_D5

void startup_callback(void) {
  //printf("Started\r\n");
  shtc3_init();
  riotee_gpio_cfg_output(PIN_MICROPHONE_DISABLE);
  riotee_gpio_set(PIN_MICROPHONE_DISABLE);
}

/* This gets called after every reset */
void reset_callback(void) {
  printf("  Reset\r\n");
}

void turnoff_callback(void) {
  printf("    Turnoff\r\n");
  /* Disable the microphone */
  riotee_gpio_set(PIN_MICROPHONE_DISABLE);
}

int main(void) {
  int counter = 0;
  printf("Main entered\r\n");

  for (;;) {

    //if (counter % 1) riotee_gpio_clear(PIN_MICROPHONE_DISABLE);
    //else riotee_gpio_set(PIN_MICROPHONE_DISABLE);
    riotee_sleep_ticks(70);
    riotee_wait_cap_charged();
    printf("C%d, ", counter++);
    //riotee_sleep_ms(200);
  }
}
