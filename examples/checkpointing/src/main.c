
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "printf.h"

int main(void) {
  unsigned int counter = 0;
  for (;;) {
    printf("Counter: %u\r\n", counter++);
    riotee_sleep_ms(100);
  }
}
