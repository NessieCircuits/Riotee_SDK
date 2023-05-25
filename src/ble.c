#include "nrf.h"

#include "nrf_ppi.h"
#include "nrf_radio.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ble.h"
#include "radio.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "runtime.h"

/* Bluetooth Core Spec 5.2 Section 2.1.2 */
#define ADV_CHANNEL_AA 0x8E89BED6

static adv_pck_t adv_pkt;

/* pointer to custom user data within adv_pkt */
static uint8_t *adv_data_address;

/* length of custom user data within adv_pkt */
static size_t adv_data_len;

static unsigned int adv_chs[] = {37, 38, 39};
static unsigned int current_adv_ch_idx;

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

int ble_prepare_adv(ble_ll_addr_t *adv_addr, const char adv_name[], size_t name_len, size_t data_len) {
  adv_pkt.header.pdu_type = ADV_NONCONN_IND;
  adv_pkt.header.txadd = 1;
  memcpy((char *)&adv_pkt.adv_addr.addr_bytes, (char *)adv_addr->addr_bytes, 6);

  /* Length of advertising mode field */
  adv_pkt.payload[0] = 0x02;
  /* Type for advertising mode field */
  adv_pkt.payload[1] = 0x01;
  /* BR/EDR not supported | LE general discoverability mode */
  adv_pkt.payload[2] = (1UL << 2) | (1UL << 1);
  /* Length of name field */
  adv_pkt.payload[3] = name_len + 1;
  /* Type for name field */
  adv_pkt.payload[4] = 0x09;
  memcpy(&adv_pkt.payload[5], adv_name, name_len);
  /* MNF data length: 3B header + length of custom data */
  adv_pkt.payload[5 + name_len] = 3 + data_len;
  /* Type for MNF-specific data field */
  adv_pkt.payload[5 + name_len + 1] = 0xFF;
  /* Two byte company ID (Nordic Semiconductor) */
  adv_pkt.payload[5 + name_len + 2] = 0x59;
  adv_pkt.payload[5 + name_len + 3] = 0x00;

  adv_data_address = &adv_pkt.payload[5 + name_len + 4];
  adv_data_len = data_len;

  adv_pkt.header.len = sizeof(ble_ll_addr_t) + 5 + name_len + 4 + data_len;

  return 0;
}

/* Configure the radio for the given BLE channel index */
static inline int ble_set_channel(unsigned int adv_ch_idx) {
  NRF_RADIO->FREQUENCY = ch2freq(adv_chs[adv_ch_idx]);
  /* Whitening initialization see Bluetooth Core Spec 5.2 Section 3.2 */
  NRF_RADIO->DATAWHITEIV = adv_chs[adv_ch_idx] & 0x3F;
  return 0;
}

int ble_advertise(void *data, adv_ch_t ch) {
  unsigned long notification_value;
  if (ch == ADV_CH_ALL) {
    adv_chs[0] = 39;
    adv_chs[1] = 38;
    adv_chs[2] = 37;
    current_adv_ch_idx = 2;
  } else {
    adv_chs[0] = ch;
    current_adv_ch_idx = 0;
  }

  ble_set_channel(current_adv_ch_idx);

  memcpy(adv_data_address, data, adv_data_len);
  radio_start();

  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  if (notification_value != USR_EVT_BLE)
    return -1;

  return 0;
}

/* Gets called after a single packet has been transmitted and the radio has been disabled (via short) */
void radio_disabled_callback() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  /* If we still have channels to advertise on */
  if (current_adv_ch_idx > 0) {
    ble_set_channel(--current_adv_ch_idx);
    NRF_RADIO->TASKS_TXEN = 1;
  } else {
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, USR_EVT_BLE, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int ble_init() {
  /* 4dBm TX power */
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);

  /* BLE 1MBit */
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

  return 0;
}
