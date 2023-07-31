#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"

#define PIN_BUTTON PIN_D6

void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
}

int main(void) {
  for (;;) {
    /* Wait for low level on button */
    riotee_gpio_wait_level(PIN_BUTTON, RIOTEE_GPIO_LEVEL_LOW, RIOTEE_GPIO_IN_PULLUP);
    /* Disable the pullup to save energy */
    riotee_gpio_cfg_disable(PIN_BUTTON);

    /* Blink LED for ~150us */
    riotee_gpio_set(PIN_LED_CTRL);
    riotee_sleep_ticks(5);
    riotee_gpio_clear(PIN_LED_CTRL);
    /* Wait until capacitor is recharged */
    riotee_wait_cap_charged();
  }
}