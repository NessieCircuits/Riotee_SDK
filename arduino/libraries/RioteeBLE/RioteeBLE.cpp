#include "RioteeBLE.h"
#include "riotee_ble.h"

#include <cstring>

riotee_ble_adv_addr_t adv_address = {.addr_bytes = {0xBE, 0xEF, 0xDE, 0xAD, 0x00, 0x01}};

void RioteeBLE::begin(const char adv_name[], void *data, size_t data_len) {
  riotee_ble_init();
  riotee_ble_adv_cfg_t cfg = {.addr = &adv_address,
                              .name = adv_name,
                              .name_len = strlen(adv_name) - 1,
                              .data = data,
                              .data_len = data_len,
                              .manufacturer_id = RIOTEE_BLE_ADV_MNF_NORDIC};
  riotee_ble_adv_cfg(&cfg);
}

void RioteeBLE::advertise() {
  riotee_ble_advertise(ADV_CH_ALL);
}

RioteeBLE BLE;