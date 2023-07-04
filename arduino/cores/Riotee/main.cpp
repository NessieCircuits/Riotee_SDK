#include "FreeRTOS.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "semphr.h"
#include "task.h"

#include "printf.h"
#include "riotee.h"
#include "riotee_adc.h"
#include "riotee_ble.h"
#include "riotee_gpint.h"
#include "riotee_timing.h"
#include "riotee_uart.h"

void startup(void);
void bootstrap(void);
void setup(void);
void loop(void);

__attribute__((weak)) void startup(void){};
__attribute__((weak)) void bootstrap(void){};

/* This gets called very early, before setting up memory */
void startup_callback(void) { startup(); }

/* This gets called one time after flashing new firmware */
void bootstrap_callback(void) { bootstrap(); }

/* This gets called after every reset */
void reset_callback(void) {
  riotee_adc_init();
  setup();
}

int main(void) {
  for (;;) {
    loop();
  }
}
