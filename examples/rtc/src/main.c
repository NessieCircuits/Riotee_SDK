
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_am1805.h"
#include "printf.h"

void lateinit(void) {
  int rc;
  riotee_gpio_cfg_output(PIN_LED_CTRL);

  /* This may take a few seconds.. see datasheet. */
  if ((rc = riotee_am1805_init()) != 0) {
    printf("Error initializing RTC: %d\r\n", rc);
  }

  if ((rc = riotee_am1805_enable_trickle()) != 0) {
    printf("Error enabling trickle charging: %d\r\n", rc);
  }
}

int main(void) {
  struct tm now;

  for (;;) {
    riotee_am1805_get_datetime(&now);
    printf("Now: %02d:%02d:%02d\r\n", now.tm_hour, now.tm_min, now.tm_sec);
    riotee_sleep_ms(1000);
  }
}
