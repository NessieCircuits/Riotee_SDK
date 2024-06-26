
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_ble.h"

#include "shtc3.h"
#include "printf.h"

const uint8_t adv_address[] = {0x01, 0xEE, 0xC0, 0xFF, 0x03, 0x02};
const char adv_name[] = "RIOTEE";

static shtc3_res_t th_result;

void earlyinit(void) {
  /* Call this early to put SHTC3 into low power mode */
  shtc3_init();
}

void lateinit(void) {
  riotee_ble_init();
}

int main(void) {
  riotee_ble_adv_cfg_t adv_cfg = {.addr = adv_address,
                                  .name = adv_name,
                                  .name_len = 6,
                                  .data = &th_result,
                                  .data_len = sizeof(shtc3_res_t),
                                  .manufacturer_id = RIOTEE_BLE_ADV_MNF_NORDIC};
  riotee_ble_adv_cfg(&adv_cfg);

  for (;;) {
    riotee_wait_cap_charged();
    shtc3_read(&th_result);
    riotee_ble_advertise(ADV_CH_ALL);
    printf("Temperature: %.2f°C, Humidity: %.2f%%\r\n", th_result.temp, th_result.humidity);
    riotee_sleep_ms(500);
  }
}
