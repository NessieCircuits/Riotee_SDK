
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "printf.h"

static unsigned int counter = 0;

/* Executes after every reset */
void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
  printf("reset\r\n");
}

/* Executes when capacitor voltage gets low */
void suspend_callback(void) {
  riotee_gpio_clear(PIN_LED_CTRL);
  printf("suspend\r\n");
}

/* Executes when capacitor voltage has recovered and after reset */
void resume_callback(void) {
  riotee_gpio_set(PIN_LED_CTRL);
  printf("resume\r\n");
}

/* Executes once after programming the device */
void bootstrap_callback(void) {
  printf("bootstrap\r\n");
}

int main(void) {
  riotee_gpio_set(PIN_LED_CTRL);
  printf("main start\r\n");
  for (;;) {
    riotee_sleep_ms(1000);
    printf("main loop %u\r\n", counter++);
  }
}
