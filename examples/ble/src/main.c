
#include "riotee.h"
#include "riotee_ble.h"
#include "riotee_timing.h"

riotee_ble_adv_addr_t adv_address = {.addr_bytes = {0x01, 0xEE, 0xC0, 0xFF, 0x03, 0x02}};
const char adv_name[] = "RIOTEE";

static struct {
  unsigned int counter;
} ble_data;

void reset_callback(void) {
  riotee_ble_init();
  riotee_ble_adv_cfg_t adv_cfg = {.addr = &adv_address,
                                  .name = adv_name,
                                  .name_len = 6,
                                  .data = &ble_data,
                                  .data_len = sizeof(ble_data),
                                  .manufacturer_id = RIOTEE_BLE_ADV_MNF_NORDIC};
  riotee_ble_adv_cfg(&adv_cfg);
  ble_data.counter = 0;
}

int main() {
  for (;;) {
    riotee_wait_cap_charged();
    riotee_ble_advertise(ADV_CH_ALL);
    ble_data.counter++;
    /* Sleep at least 250ms before next advertising round */
    riotee_sleep_ms(250);
  }
}
