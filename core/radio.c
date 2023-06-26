#include <string.h>
#include "nrf.h"

#include "radio.h"

RADIO_CALLBACK cb_disabled = NULL;
RADIO_CALLBACK cb_txready = NULL;
RADIO_CALLBACK cb_rxready = NULL;
RADIO_CALLBACK cb_crcok = NULL;
RADIO_CALLBACK cb_crcerr = NULL;
RADIO_CALLBACK cb_address = NULL;

void radio_init() {
  NRF_PPI->CH[18].EEP = (uint32_t)&NRF_CLOCK->EVENTS_HFCLKSTARTED;
  NRF_PPI->CH[18].TEP = (uint32_t)&NRF_RADIO->TASKS_TXEN;

  NVIC_EnableIRQ(RADIO_IRQn);
}

void radio_start() {
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
}

void radio_stop() {
  NRF_RADIO->TASKS_DISABLE = 1;
  NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}

int radio_cb_register(radio_evt_t evt, RADIO_CALLBACK cb) {
  switch (evt) {
    case RADIO_EVT_DISABLED:
      cb_disabled = cb;
      NRF_RADIO->EVENTS_DISABLED = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_DISABLED_Msk;
      break;
    case RADIO_EVT_RXREADY:
      cb_rxready = cb;
      NRF_RADIO->EVENTS_RXREADY = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_RXREADY_Msk;
      break;
    case RADIO_EVT_TXREADY:
      cb_txready = cb;
      NRF_RADIO->EVENTS_TXREADY = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_TXREADY_Msk;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = cb;
      NRF_RADIO->EVENTS_CRCOK = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_CRCOK_Msk;
      break;
    case RADIO_EVT_CRCERR:
      cb_crcerr = cb;
      NRF_RADIO->EVENTS_CRCERROR = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_CRCERROR_Msk;
      break;
    case RADIO_EVT_ADDRESS:
      cb_address = cb;
      NRF_RADIO->EVENTS_ADDRESS = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_ADDRESS_Msk;
      break;
    default:
      return -1;
  }
  return 0;
}

int radio_cb_unregister(radio_evt_t evt) {
  switch (evt) {
    case RADIO_EVT_DISABLED:
      cb_disabled = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_DISABLED_Msk;
      break;
    case RADIO_EVT_RXREADY:
      cb_rxready = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_RXREADY_Msk;
      break;
    case RADIO_EVT_TXREADY:
      cb_txready = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_TXREADY_Msk;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_CRCOK_Msk;
      break;
    case RADIO_EVT_CRCERR:
      cb_crcerr = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_CRCERROR_Msk;
      break;
    case RADIO_EVT_ADDRESS:
      cb_address = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_ADDRESS_Msk;
      break;
    default:
      return -1;
  }
  return 0;
}

void RADIO_IRQHandler(void) {
  if (NRF_RADIO->EVENTS_DISABLED == 1) {
    NRF_RADIO->EVENTS_DISABLED = 0;
    if (cb_disabled != NULL)
      cb_disabled();
  }
  if (NRF_RADIO->EVENTS_RXREADY == 1) {
    NRF_RADIO->EVENTS_RXREADY = 0;
    if (cb_rxready != NULL)
      cb_rxready();
  }
  if (NRF_RADIO->EVENTS_TXREADY == 1) {
    NRF_RADIO->EVENTS_TXREADY = 0;
    if (cb_txready != NULL)
      cb_txready();
  }
  if (NRF_RADIO->EVENTS_CRCOK == 1) {
    NRF_RADIO->EVENTS_CRCOK = 0;
    if (cb_crcok != NULL)
      cb_crcok();
  }
  if (NRF_RADIO->EVENTS_CRCERROR == 1) {
    NRF_RADIO->EVENTS_CRCERROR = 0;
    if (cb_crcerr != NULL)
      cb_crcerr();
  }
  if (NRF_RADIO->EVENTS_ADDRESS == 1) {
    NRF_RADIO->EVENTS_ADDRESS = 0;
    if (cb_address != NULL)
      cb_address();
  }
}
