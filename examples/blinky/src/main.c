
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"

/* This gets called after every reset */
void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
}

/* This gets called when capacitor voltage gets low */
void turnoff_callback(void) {
  riotee_gpio_clear(PIN_LED_CTRL);
}

int main(void) {
  for (;;) {
    riotee_wait_cap_charged();
    riotee_gpio_set(PIN_LED_CTRL);
  }
}
