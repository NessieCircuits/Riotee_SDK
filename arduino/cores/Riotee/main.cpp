#include "riotee.h"
#include "riotee_adc.h"

void startup(void);
void bootstrap(void);
void setup(void);
void loop(void);

__attribute__((weak)) void startup(void){};
__attribute__((weak)) void bootstrap(void){};

/* This gets called very early, before setting up memory */
void startup_callback(void) {
  startup();
}

/* This gets called one time after flashing new firmware */
void bootstrap_callback(void) {
  bootstrap();
}

/* This gets called after every reset */
void reset_callback(void) {
  riotee_adc_init();
  setup();
}

int main(void) {
  for (;;) {
    loop();
  }
}
