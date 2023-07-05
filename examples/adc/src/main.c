
#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_adc.h"
#include "printf.h"

void reset_callback(void) {
  riotee_adc_init();
}

int main(void) {
  for (;;) {
    int16_t val = riotee_adc_read(RIOTEE_ADC_INPUT_A0);
    printf("ADC value: %d\r\n", val);
    riotee_sleep_ms(1000);
  }
}
