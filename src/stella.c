#include <string.h>
#include "nrfx.h"
#include "FreeRTOS.h"

#include "riotee_module_pins.h"
#include "stella.h"
#include "radio.h"
#include "runtime.h"
#include "printf.h"

static pkt_t rx_buf;
static pkt_t tx_buf;

static pkt_t *rx_buf_ptr;

/* Counts number of transmitted packets */
static uint16_t pkt_counter = 0;

enum {
  /* Uplink logical address index */
  LA_UPLINK_IDX = 1,
  /* Downlink logical address index */
  LA_DOWNLINK_IDX = 2,
};

enum {
  /* Uplink logical address index */
  LA_UPLINK = 0x5D,
  /* Downlink logical address index */
  LA_DOWNLINK = 0xF7,
};

static void radio_crc_ok(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  radio_stop();
  /* Stop the timeout timer */
  NRF_TIMER2->TASKS_STOP = 1;
  xTaskNotifyIndexedFromISR(usr_task_handle, 1, USR_EVT_STELLA_RCVD, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
static void radio_txready(void) {
  /* Now that radio is transmitting, point the double-buffered packet pointer to the rx buffer*/
  NRF_RADIO->PACKETPTR = (uint32_t)rx_buf_ptr;
}

static void radio_rxready(void) {
  NRF_RADIO->SHORTS &= ~(RADIO_SHORTS_DISABLED_RXEN_Msk);

  NRF_TIMER2->TASKS_CLEAR = 1;
  NRF_TIMER2->TASKS_START = 1;
}

/* Initializes a timer used for timing out reception of an acknowledgement */
static int timer_init(void) {
  /* 1us period */
  NRF_TIMER2->PRESCALER = 4;
  NRF_TIMER2->CC[0] = 320;
  NRF_TIMER2->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
  NRF_TIMER2->SHORTS |= TIMER_SHORTS_COMPARE0_STOP_Msk;
  NVIC_EnableIRQ(TIMER2_IRQn);
  return 0;
}

int stella_init() {
  /* 0dBm TX power */
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
  /* 2476 MHz frequency */
  NRF_RADIO->FREQUENCY = 76UL;
  /* BLE 2MBit */
  NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
  /* Fast radio rampup */
  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);

  /* We'll only use base address 1, i.e. logical addresses 1-7 */
  NRF_RADIO->BASE1 = 0xFB235D41;

  NRF_RADIO->PREFIX0 = (LA_DOWNLINK << 16) | (LA_UPLINK << 8);

  /* We want to receive on downlink logical addresses */
  NRF_RADIO->RXADDRESSES = (1UL << LA_DOWNLINK_IDX);

  /* And send on uplink logical address */
  NRF_RADIO->TXADDRESS = LA_UPLINK_IDX;
  /* No S0, LEN and S1 fields */
  NRF_RADIO->PCNF0 = (0 << RADIO_PCNF0_S1LEN_Pos) | (0 << RADIO_PCNF0_S0LEN_Pos) | (8 << RADIO_PCNF0_LFLEN_Pos) |
                     (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos);

  /* No whitening, little endian, 2B base address, 4B payload */
  NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |
                     (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) | (2 << RADIO_PCNF1_BALEN_Pos) |
                     (0 << RADIO_PCNF1_STATLEN_Pos) | (255 << RADIO_PCNF1_MAXLEN_Pos);

  /* One byte CRC */
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos);
  NRF_RADIO->CRCINIT = 0xABUL;
  NRF_RADIO->CRCPOLY = 0x108UL;

  /* Set default shorts */
  NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk;

  radio_init();
  timer_init();

  radio_cb_register(RADIO_EVT_CRCOK, radio_crc_ok);
  radio_cb_register(RADIO_EVT_RXREADY, radio_rxready);
  radio_cb_register(RADIO_EVT_TXREADY, radio_txready);
  NRF_PPI->CHENSET = PPI_CHENSET_CH18_Msk;

  tx_buf.dev_id = NRF_FICR->DEVICEADDR[0];

  return 0;
}

void TIMER2_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_TIMER2->EVENTS_COMPARE[0] == 1) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    radio_stop();
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, USR_EVT_STELLA_TIMEOUT, eSetValueWithOverwrite,
                              &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int stella_transceive(pkt_t *rx_pkt, pkt_t *tx_pkt) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  /* Packet transmission will start automatically when HFXO is running */
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  rx_buf_ptr = rx_pkt;
  NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RXEN_Msk;
  NRF_RADIO->PACKETPTR = (uint32_t)tx_pkt;

  xTaskNotifyStateClear(usr_task_handle);
  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value == USR_EVT_RESET)
    return STELLA_ERR_RESET;

  if (notification_value == USR_EVT_STELLA_TIMEOUT)
    return STELLA_ERR_NOACK;

  if (rx_pkt->acknowledgement_id != tx_pkt->pkt_id)
    return STELLA_ERR_GENERIC;

  return STELLA_ERR_OK;
}

int stella_send(uint8_t *data, size_t n) {
  memcpy(tx_buf.data, data, n);
  /* Packet length is payload length plus eight byte header */
  tx_buf.len = n + 8;
  tx_buf.pkt_id = pkt_counter++;
  return stella_transceive(&rx_buf, &tx_buf);
}
