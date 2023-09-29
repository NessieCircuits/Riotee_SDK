
#include "riotee.h"
#include "riotee_gpio.h"
#include "led.hpp"
#include "printf.h"

static LED led = LED(PIN_LED_CTRL);

/* This gets called when capacitor voltage gets low */
void suspend_callback(void) {
  led.off();
}

int main(void) {
  for (;;) {
    riotee_wait_cap_charged();
    led.on();
    printf("%u", led._pin);
  }
}
