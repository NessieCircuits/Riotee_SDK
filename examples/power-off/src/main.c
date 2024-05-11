
#include "nrf.h"

#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_thresholds.h"
#include "riotee_am1805.h"
#include "printf.h"

/* This gets called after every reset */
void reset_callback(void) {
  int rc;
  if ((rc = riotee_am1805_init()) != 0)
    printf("Error initializing RTC: %d\r\n", rc);
}

void am1805_poweroff(unsigned int seconds) {
  int rc;
  struct tm date_off;

  /* Read current date from the RTC. */
  riotee_am1805_get_datetime(&date_off);
  /* Add seconds. */
  date_off.tm_sec += seconds;
  /* Ensure that this is a correct date. */
  mktime(&date_off);

  /* Set an alarm that will re-enable the power supply. */
  riotee_am1805_set_alarm(&date_off);
  printf("Power off\r\n");
  /* Disable the power supply with the p-MOSFET. */
  riotee_am1805_disable_power();
}

int main(void) {
  struct tm now;

  riotee_am1805_get_datetime(&now);
  printf("Wakeup at %02d:%02d:%02d\r\n", now.tm_hour, now.tm_min, now.tm_sec);

  am1805_poweroff(30);
  while (1) {
    printf("Alive..\r\n");
  }
}
