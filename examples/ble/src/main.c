
#include "riotee.h"
#include "riotee_ble.h"
#include "riotee_timing.h"

riotee_ble_ll_addr_t adv_address = {.addr_bytes = {0x01, 0xEE, 0xC0, 0xFF, 0x03, 0x02}};

static struct {
  unsigned int counter;
} ble_data;

void reset_callback(void) {
  riotee_ble_init();
  riotee_ble_prepare_adv(&adv_address, "RIOTEE", 6, sizeof(ble_data));
  ble_data.counter = 0;
}

int main() {
  for (;;) {
    riotee_wait_cap_charged();
    riotee_ble_advertise(&ble_data, ADV_CH_ALL);
    ble_data.counter++;
    /* Sleep at least 250ms before next advertising round */
    riotee_sleep_ms(250);
  }
}
