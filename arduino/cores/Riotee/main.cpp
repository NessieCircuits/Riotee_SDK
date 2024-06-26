#include "riotee.h"
#include "riotee_adc.h"

#include "external/ArduinoCore-API/api/Common.h"

void startup(void);
void bootstrap(void);

__attribute__((weak)) void startup(void){};
__attribute__((weak)) void bootstrap(void){};

/* This gets called very early, before setting up memory */
void earlyinit(void) {
  startup();
}

/* This gets called one time after flashing new firmware */
void bootstrap(void) {
  bootstrap();
}

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
