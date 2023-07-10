#include "riotee.h"
#include "riotee_timing.h"
#include "printf.h"
#include "riotee_uart.h"
#include "riotee_stella.h"
#include "riotee_adc.h"
#include "riotee_gpio.h"

#include "shtc3.h"
#include "vm1010.h"

/* Pin D10 enables/disables microphone on the Riotee Sensor Shield */
#define PIN_MICROPHONE_ENABLE PIN_D5

void startup_callback(void) {
  /* Call this early to put SHTC3 into low power mode */
  shtc3_init();
  /* Disable microphone to reduce current consumption. */
  riotee_gpio_cfg_output(PIN_MICROPHONE_ENABLE);
  riotee_gpio_set(PIN_MICROPHONE_ENABLE);
}

/* This gets called after every reset */
void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);

  riotee_stella_init();
  /* Required for VM1010 */
  riotee_adc_init();

  vm1010_cfg_t cfg = {.pin_vout = PIN_D2, .pin_vbias = PIN_D3, .pin_mode = PIN_D10, .pin_dout = PIN_D4};
  vm1010_init(&cfg);
}

void turnoff_callback(void) {
  riotee_gpio_clear(PIN_LED_CTRL);
  /* Disable the microphone */
  riotee_gpio_set(PIN_MICROPHONE_ENABLE);
}

int16_t samples[120];

int main(void) {
  int rc;

  printf("Startup!");
  for (;;) {
    riotee_wait_cap_charged();

    /* Switch on microphone */
    riotee_gpio_clear(PIN_MICROPHONE_ENABLE);
    /* Wait for 2ms for V_BIAS to come up */
    riotee_sleep_ticks(70);
    /* Wait for wake-on-sound signal from microphone */
    vm1010_wait4sound();
    /* Wait until microphone can be sampled (see VM1010 datasheet)*/
    riotee_sleep_ticks(5);
    printf("Sampling..");
    rc = vm1010_sample(samples, 120, 4);
    printf("Done: %d", rc);
    /* Disable the microphone */
    riotee_gpio_set(PIN_MICROPHONE_ENABLE);
    /* Re-charge */
    riotee_wait_cap_charged();

    printf("Sending..");
    riotee_stella_send((uint8_t *)samples, sizeof(samples) * sizeof(int16_t));
    printf("Done: %d", rc);
  }
}
