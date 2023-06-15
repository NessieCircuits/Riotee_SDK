#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "riotee_timing.h"
#include "riotee_gpint.h"
#include "printf.h"
#include "riotee.h"
#include "runtime.h"
#include "riotee_thresholds.h"
#include "riotee_uart.h"
#include "riotee_ble.h"
#include "riotee_adc.h"

riotee_ble_ll_addr_t adv_address = {.addr_bytes = {0xBE, 0xEF, 0xDE, 0xAD, 0x00, 0x01}};

static struct {
  uint32_t counter;
} ble_data;

static void led_blink(unsigned int us) {
  taskENTER_CRITICAL();
  nrf_gpio_pin_set(PIN_LED_CTRL);
  riotee_delay_us(us);
  nrf_gpio_pin_clear(PIN_LED_CTRL);
  taskEXIT_CRITICAL();
}

/* This gets called one time after flashing new firmware */
void bootstrap_callback(void) {
  printf("All new!");
}

/* This gets called after every reset */
void reset_callback(void) {
  nrf_gpio_cfg_output(PIN_LED_CTRL);

  riotee_ble_init();
  riotee_ble_prepare_adv(&adv_address, "RIOTEE", 6, sizeof(ble_data));
  ble_data.counter = 0;
}

void user_task(void *pvParameter) {
  UNUSED_PARAMETER(pvParameter);
  for (;;) {
    wait_until_charged();
    led_blink(250);
    riotee_sleep_ms(500);
    riotee_ble_advertise(&ble_data, ADV_CH_ALL);
    ble_data.counter++;
  }
}
