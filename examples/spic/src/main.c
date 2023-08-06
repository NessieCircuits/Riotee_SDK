#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_spic.h"
#include "printf.h"

uint8_t tx_data[] = {0xCA, 0xFE, 0xD0, 0x0D};
uint8_t rx_data[sizeof(tx_data)];

void reset_callback(void) {
  riotee_spic_cfg_t cfg = {.frequency = RIOTEE_SPIC_FREQUENCY_M8,
                           .mode = RIOTEE_SPIC_MODE0_CPOL0_CPHA0,
                           .order = RIOTEE_SPIC_ORDER_LSBFIRST,
                           .pin_cs = PIN_D7,
                           .pin_sck = PIN_D8,
                           .pin_copi = PIN_D10,
                           .pin_cipo = PIN_D9};
  riotee_spic_init(&cfg);
}

int main(void) {
  for (;;) {
    riotee_spic_transfer(tx_data, sizeof(tx_data), rx_data, sizeof(rx_data));
    for (int i = 0; i < 4; i++) {
      printf("%02X", rx_data[i]);
    }
    printf("\r\n");
    riotee_sleep_ms(1000);
  }
}