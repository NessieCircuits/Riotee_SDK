
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

void bootstrap_callback(void) {
  /* Reset RTC once after programming. */
  riotee_am1805_reset();
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
  printf("Setting alarm at %02d:%02d:%02d\r\n", date_off.tm_hour, date_off.tm_min, date_off.tm_sec);

  /* Clear interrupt from previous alarm. */
  riotee_am1805_clear_alarm();

  /* Set an alarm that will re-enable the power supply. */
  riotee_am1805_set_alarm(&date_off);
  printf("Power off\r\n");

  /* Disable the power supply with the p-MOSFET. */
  if ((rc = riotee_am1805_disable_power()) != 0)
    printf("Error disabling power: %d\r\n", rc);
}

int main(void) {
  struct tm now;

  riotee_am1805_get_datetime(&now);
  printf("Startup at %02d:%02d:%02d\r\n", now.tm_hour, now.tm_min, now.tm_sec);

  while (1) {
    riotee_wait_cap_charged();
    /* Checkpoint here -> will wake up here after power is restored*/
    riotee_checkpoint();
    riotee_am1805_get_datetime(&now);
    printf("It is now %02d:%02d:%02d\r\n", now.tm_hour, now.tm_min, now.tm_sec);
    am1805_poweroff(15);
    riotee_sleep_ms(1000);
  }
}
