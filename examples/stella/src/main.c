
#include "riotee.h"
#include "riotee_stella.h"
#include "riotee_timing.h"
#include "riotee_gpio.h"

static unsigned int counter = 0;

void reset_callback(void) {
  riotee_stella_init();
  riotee_gpio_cfg_output(PIN_D5);
}

int main() {
  for (;;) {
    riotee_stella_send(&counter, sizeof(counter));
    counter++;
    riotee_gpio_set(PIN_D5);
    riotee_sleep_ms(500);
    riotee_gpio_clear(PIN_D5);
    riotee_sleep_ms(500);
  }
}
