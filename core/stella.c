#include <string.h>
#include "nrfx.h"
#include "FreeRTOS.h"

#include "riotee.h"
#include "riotee_stella.h"
#include "radio.h"
#include "runtime.h"

static riotee_stella_pkt_t _rx_pkt_buf __attribute__((section(".retained_bss")));
static riotee_stella_pkt_t _tx_pkt_buf __attribute__((section(".retained_bss")));
static riotee_stella_pkt_t *rx_buf_ptr __attribute__((section(".retained_bss")));

/* Counts number of transmitted packets */
static unsigned int pkt_counter __attribute__((section(".retained_bss")));

enum {
  EVT_STELLA_TIMEOUT = EVT_STELLA_BASE + 0,
  EVT_STELLA_RCVD = EVT_STELLA_BASE + 1,
  EVT_STELLA_CRCERR = EVT_STELLA_BASE + 2,
};

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

static uint32_t _dev_id __attribute__((section(".retained_bss")));

TEARDOWN_FUN(stella_teardown_ptr);

/* Valid acknowledgment received */
static void radio_crc_ok(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  radio_stop();
  stella_teardown_ptr = NULL;
  xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_STELLA_RCVD, eSetBits, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* Invalid acknowledgment received */
static void radio_crc_err(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  radio_stop();
  stella_teardown_ptr = NULL;
  xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_STELLA_CRCERR, eSetBits, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* Address received */
static void radio_address(void) {
  /* Stop the timeout timer */
  NRF_TIMER2->TASKS_STOP = 1;
  /* nRF52833 errata [78] */
  NRF_TIMER2->TASKS_SHUTDOWN = 1;
  radio_cb_unregister(RADIO_EVT_ADDRESS);
}

static void radio_txready(void) {
  /* Now that radio is transmitting, point the double-buffered packet pointer to the rx buffer*/
  NRF_RADIO->PACKETPTR = (uint32_t)rx_buf_ptr;
}

/* Radio has ramped up for reception of an acknowledgment */
static void radio_rxready(void) {
  NRF_RADIO->SHORTS &= ~(RADIO_SHORTS_DISABLED_RXEN_Msk);

  /* Notify us when an address is received */
  radio_cb_register(RADIO_EVT_ADDRESS, radio_address);
  /* Set a timeout for reception of an address */
  NRF_TIMER2->TASKS_CLEAR = 1;
  NRF_TIMER2->TASKS_START = 1;
}

/* Initializes a timer used for timing out reception of an acknowledgment */
static int timer_init(void) {
  /* 1us period */
  NRF_TIMER2->PRESCALER = 4;
  /* 100us timeout for receiving the address of the acknowledgment */
  NRF_TIMER2->CC[0] = 100;
  NRF_TIMER2->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
  NVIC_EnableIRQ(TIMER2_IRQn);
  return 0;
}

void riotee_stella_init() {
  /* 0dBm TX power */
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
  /* 2476 MHz frequency */
  NRF_RADIO->FREQUENCY = 76UL;
  /* BLE 1MBit */
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

  /* Set the default device ID */
  riotee_stella_set_id(NRF_FICR->DEVICEADDR[0]);

  radio_init();
  timer_init();

  radio_cb_register(RADIO_EVT_CRCOK, radio_crc_ok);
  radio_cb_register(RADIO_EVT_CRCERR, radio_crc_err);
  radio_cb_register(RADIO_EVT_RXREADY, radio_rxready);
  radio_cb_register(RADIO_EVT_TXREADY, radio_txready);

  NRF_PPI->CHENSET = PPI_CHENSET_CH18_Msk;
}

/* Timeout for reception of the acknowledgment */
void TIMER2_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_TIMER2->EVENTS_COMPARE[0] == 1) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    radio_cb_unregister(RADIO_EVT_ADDRESS);
    radio_stop();
    stella_teardown_ptr = NULL;
    NRF_TIMER2->TASKS_STOP = 1;
    /* nRF52833 errata [78] */
    NRF_TIMER2->TASKS_SHUTDOWN = 1;

    xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_STELLA_TIMEOUT, eSetBits, &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void teardown(void) {
  radio_stop();
  radio_cb_unregister(RADIO_EVT_ADDRESS);
  NRF_TIMER2->TASKS_STOP = 1;
  xTaskNotifyIndexed(usr_task_handle, 1, EVT_TEARDOWN, eSetBits);
  stella_teardown_ptr = NULL;
}

static inline int wait_for_completion(riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt) {
  unsigned long notification_value;

  /* Wait until acknowledgment is received/expired */
  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  /* Count the packet whether successful or not. */
  pkt_counter++;

  /* Make sure HFXO has stopped so the next packet can be sent right after returning. */
  while ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk) == CLOCK_HFCLKSTAT_SRC_Xtal) {
  }

  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;

  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;

  if (notification_value == EVT_STELLA_CRCERR)
    return RIOTEE_ERR_STELLA_NOACK;

  if (notification_value == EVT_STELLA_TIMEOUT)
    return RIOTEE_ERR_STELLA_NOACK;

  if (notification_value != EVT_STELLA_RCVD)
    return RIOTEE_ERR_GENERIC;

  /* acknowledgment ID must match packet ID */
  if (rx_pkt->hdr.ack_id != tx_pkt->hdr.pkt_id)
    return RIOTEE_ERR_STELLA_INVALIDACK;

  /* acknowledgment always contains device's ID */
  if (rx_pkt->hdr.dev_id != tx_pkt->hdr.dev_id)
    return RIOTEE_ERR_STELLA_INVALIDACK;

  return RIOTEE_SUCCESS;
}

static inline riotee_rc_t _transceive(riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  /* Packet transmission will start automatically when HFXO is running */
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RXEN_Msk;
  NRF_RADIO->PACKETPTR = (uint32_t)tx_pkt;
  /* This will be moved into PACKETPTR after TX packet has been sent. */
  rx_buf_ptr = rx_pkt;

  xTaskNotifyStateClearIndexed(usr_task_handle, 1);
  ulTaskNotifyValueClearIndexed(usr_task_handle, 1, 0xFFFFFFFF);

  stella_teardown_ptr = teardown;
  taskEXIT_CRITICAL();

  /* Wait until acknowledgment is received/expired */
  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  /* Count the packet whether successful or not. */
  pkt_counter++;

  /* Make sure HFXO has stopped so the next packet can be sent right after returning. */
  while ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk) == CLOCK_HFCLKSTAT_SRC_Xtal) {
  }

  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;

  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;

  if (notification_value == EVT_STELLA_CRCERR)
    return RIOTEE_ERR_STELLA_NOACK;

  if (notification_value == EVT_STELLA_TIMEOUT)
    return RIOTEE_ERR_STELLA_NOACK;

  if (notification_value != EVT_STELLA_RCVD)
    return RIOTEE_ERR_GENERIC;

  /* acknowledgment ID must match packet ID */
  if (rx_pkt->hdr.ack_id != tx_pkt->hdr.pkt_id)
    return RIOTEE_ERR_STELLA_INVALIDACK;

  /* acknowledgment always contains device's ID */
  if (rx_pkt->hdr.dev_id != tx_pkt->hdr.dev_id)
    return RIOTEE_ERR_STELLA_INVALIDACK;

  return RIOTEE_SUCCESS;
}

riotee_rc_t riotee_stella_transceive(uint8_t *rx_buf, size_t rx_size, void *tx_data, size_t tx_size) {
  if (tx_size > RIOTEE_STELLA_MAX_DATA)
    return RIOTEE_ERR_OVERFLOW;

  memcpy(_tx_pkt_buf.data, tx_data, tx_size);
  _tx_pkt_buf.len = sizeof(riotee_stella_pkt_header_t) + tx_size;

  /* Packet ID is truncated packet counter. */
  _tx_pkt_buf.hdr.pkt_id = (uint16_t)pkt_counter;

  /* Set correct device ID */
  _tx_pkt_buf.hdr.dev_id = _dev_id;

  riotee_rc_t rc = _transceive(&_rx_pkt_buf, &_tx_pkt_buf);

  if (rc != RIOTEE_SUCCESS)
    return rc;

  size_t payload_size = _rx_pkt_buf.len - sizeof(riotee_stella_pkt_header_t);

  if (rx_size < payload_size)
    return RIOTEE_ERR_OVERFLOW;

  memcpy(rx_buf, _rx_pkt_buf.data, payload_size);

  return payload_size;
}

riotee_rc_t riotee_stella_send(void *tx_data, size_t tx_size) {
  return riotee_stella_transceive(NULL, 0, tx_data, tx_size);
}

riotee_rc_t riotee_stella_receive(uint8_t *rx_buf, size_t rx_size) {
  return riotee_stella_transceive(rx_buf, rx_size, NULL, 0);
}

void riotee_stella_set_id(uint32_t dev_id) {
  _dev_id = dev_id;
}

uint32_t riotee_stella_get_id(void) {
  return _dev_id;
}

unsigned int riotee_stella_get_packet_counter(void) {
  return pkt_counter;
}