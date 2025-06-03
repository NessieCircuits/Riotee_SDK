#include "riotee.h"
#include "riotee_adc.h"

#include "external/ArduinoCore-API/api/Common.h"

/* This gets called after every reset */
void lateinit(void) {
  riotee_adc_init();
  setup();
}

int main(void) {
  for (;;) {
    loop();
  }
}
