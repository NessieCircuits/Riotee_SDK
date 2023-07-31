
#include "riotee.h"
#include "riotee_stella.h"
#include "riotee_timing.h"

static unsigned int counter = 0;

void reset_callback(void) {
  riotee_stella_init();
}

int main() {
  for (;;) {
    riotee_stella_send(&counter, sizeof(counter));
    counter++;
    riotee_sleep_ms(1000);
  }
}
