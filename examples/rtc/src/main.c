
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_am1805.h"
#include "printf.h"

void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
}

int main(void) {
  int rc;
  uint8_t reg_buf;
  struct tm now;

  printf("Waiting 3s..\r\n");
  riotee_sleep_ms(3000);

  if ((rc = am1805_enable_trickle()) != 0) {
    printf("Error enabling trickle charging: %d\r\n", rc);
  }

  for (;;) {
    riotee_wait_cap_charged();

    am1805_get_datetime(&now);
    printf("Now: %02d:%02d:%02d\r\n", now.tm_hour, now.tm_min, now.tm_sec);

    riotee_sleep_ms(1000);
  }
}
