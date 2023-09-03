#include "nrf.h"
#include "nrf_radio.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "riotee_ble.h"
#include "radio.h"

#include "runtime.h"
#include "printf.h"

/* Bluetooth Core Spec 5.2 Section 2.1.2 */
#define ADV_CHANNEL_AA 0x8E89BED6

static riotee_adv_pck_t adv_pkt;

/* pointer to custom user data within adv_pkt */
static uint8_t *adv_data_pkt;
/* user-provided pointer to custom user data */
static void *adv_data_usr;
/* length of custom user data within adv_pkt */
static size_t adv_data_len;

static unsigned int adv_chs[3];
static unsigned int current_adv_ch_idx;

TEARDOWN_FUN(ble_teardown_ptr);

static __inline int8_t ch2freq(uint8_t ch) {
  switch (ch) {
    case 37:
      return 2;
    case 38:
      return 26;
    case 39:
      return 80;
    default:
      if (ch > 39)
        return -1;
      else if (ch < 11)
        return 4 + (2 * ch);
      else
        return 6 + (2 * ch);
  }
}

void teardown(void) {
  radio_stop();
  ble_teardown_ptr = NULL;
  xTaskNotifyIndexed(usr_task_handle, 1, EVT_TEARDOWN, eSetBits);
}

riotee_rc_t riotee_ble_adv_cfg(riotee_ble_adv_cfg_t *cfg) {
  const unsigned int payload_free = sizeof(adv_pkt.payload) - 8;

  if (cfg->name_len > payload_free)
    return RIOTEE_ERR_OVERFLOW;
  if (cfg->data_len + cfg->name_len > payload_free)
    return RIOTEE_ERR_OVERFLOW;

  adv_pkt.header.pdu_type = ADV_NONCONN_IND;
  adv_pkt.header.txadd = 1;
  memcpy(adv_pkt.adv_addr, cfg->addr, 6);

  /* Length of advertising mode field */
  adv_pkt.payload[0] = 0x02;
  /* Type for advertising mode field */
  adv_pkt.payload[1] = 0x01;
  /* BR/EDR not supported | LE general discoverability mode */
  adv_pkt.payload[2] = (1UL << 2) | (1UL << 1);
  /* Length of name field */
  adv_pkt.payload[3] = cfg->name_len + 1;
  /* Type for name field */
  adv_pkt.payload[4] = 0x09;
  memcpy(&adv_pkt.payload[5], cfg->name, cfg->name_len);
  /* MNF data length: 3B header + length of custom data */
  adv_pkt.payload[5 + cfg->name_len] = 3 + cfg->data_len;
  /* Type for MNF-specific data field */
  adv_pkt.payload[5 + cfg->name_len + 1] = 0xFF;
  /* Two byte company ID (Nordic Semiconductor) */
  memcpy(adv_pkt.payload + 5 + cfg->name_len + 2, &cfg->manufacturer_id, 2);

  adv_data_pkt = &adv_pkt.payload[5 + cfg->name_len + 4];
  adv_data_usr = cfg->data;
  adv_data_len = cfg->data_len;

  adv_pkt.header.len = sizeof(riotee_ble_adv_addr_t) + 5 + cfg->name_len + 4 + cfg->data_len;

  return RIOTEE_SUCCESS;
}

/* Configure the radio for the given BLE channel index */
static inline int set_channel(unsigned int adv_ch_idx) {
  NRF_RADIO->FREQUENCY = ch2freq(adv_chs[adv_ch_idx]);
  /* Whitening initialization see Bluetooth Core Spec 5.2 Section 3.2 */
  NRF_RADIO->DATAWHITEIV = adv_chs[adv_ch_idx] & 0x3F;
  return 0;
}

riotee_rc_t riotee_ble_advertise(riotee_adv_ch_t ch) {
  unsigned long notification_value;

  /* Copy user data into advertisement packet */
  memcpy(adv_data_pkt, adv_data_usr, adv_data_len);

  if (ch == ADV_CH_ALL) {
    adv_chs[0] = 39;
    adv_chs[1] = 38;
    adv_chs[2] = 37;
    current_adv_ch_idx = 2;
  } else {
    adv_chs[0] = ch;
    current_adv_ch_idx = 0;
  }

  taskENTER_CRITICAL();
  set_channel(current_adv_ch_idx);

  radio_start();
  xTaskNotifyStateClearIndexed(usr_task_handle, 1);
  ulTaskNotifyValueClearIndexed(usr_task_handle, 1, 0xFFFFFFFF);

  /* Register the teardown function */
  ble_teardown_ptr = teardown;
  taskEXIT_CRITICAL();
  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;
  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;
  if (notification_value == EVT_BLE_BASE)
    return RIOTEE_SUCCESS;

  return RIOTEE_ERR_GENERIC;
}

/* Gets called after a single packet has been transmitted and the radio has been disabled (via short) */
void radio_disabled_callback() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  /* If we still have channels to advertise on */
  if (current_adv_ch_idx > 0) {
    set_channel(--current_adv_ch_idx);
    NRF_RADIO->TASKS_TXEN = 1;
  } else {
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
    /* Unregister teardown function */
    ble_teardown_ptr = NULL;
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_BLE_BASE, eSetBits, &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void riotee_ble_init(void) {
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);

  NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
  /* Fast radio rampup */
  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);

  /* Bluetooth Core Spec 5.2 Section 2.1.2 */
  NRF_RADIO->BASE0 = (ADV_CHANNEL_AA << 8) & 0xFFFFFF00;
  NRF_RADIO->PREFIX0 = (ADV_CHANNEL_AA >> 24) & RADIO_PREFIX0_AP0_Msk;

  /* Logical address 0 -> BASE0 + PREFIX.AP0 */
  NRF_RADIO->TXADDRESS = 0x00;

  /* Stores BLE header */
  NRF_RADIO->PCNF0 = (0UL << RADIO_PCNF0_S1LEN_Pos) | (1UL << RADIO_PCNF0_S0LEN_Pos) | (8UL << RADIO_PCNF0_LFLEN_Pos) |
                     (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos);

  /* Data whitening, little endian, 3B base address */
  NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos) |
                     (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) | (3 << RADIO_PCNF1_BALEN_Pos) |
                     (0 << RADIO_PCNF1_STATLEN_Pos) | (255 << RADIO_PCNF1_MAXLEN_Pos);

  /* For CRC settings see Bluetooth Core Spec 5.2 Section 3.1.1 */
  /* Three byte CRC, skip address */
  NRF_RADIO->CRCCNF =
      (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) | (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);

  /* Initial value */
  NRF_RADIO->CRCINIT = 0x555555;
  /* CRC poly: x^24 + x^10 + x^9 + x^6 + x^4 + x^3 + x + 1 */
  NRF_RADIO->CRCPOLY = 0x100065B;

  /* Set default shorts */
  NRF_RADIO->SHORTS = NRF_RADIO_SHORT_READY_START_MASK | NRF_RADIO_SHORT_END_DISABLE_MASK;

  /* Always transmit packet from the same memory address, just replace the payload there */
  NRF_RADIO->PACKETPTR = (uint32_t)&adv_pkt;

  radio_cb_register(RADIO_EVT_DISABLED, radio_disabled_callback);

  radio_init();

  /* This channel starts radio transmissions as soon as HFCLK is running*/
  NRF_PPI->CHENSET = PPI_CHEN_CH18_Msk;
}
