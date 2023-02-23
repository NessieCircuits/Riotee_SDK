#include <string.h>
#include "nrf.h"

#include "radio.h"

RADIO_CALLBACK cb_disabled = NULL;
RADIO_CALLBACK cb_ready = NULL;
RADIO_CALLBACK cb_crcok = NULL;

int radio_init() {
  NRF_PPI->CH[18].EEP = (uint32_t)&NRF_CLOCK->EVENTS_HFCLKSTARTED;
  NRF_PPI->CH[18].TEP = (uint32_t)&NRF_RADIO->TASKS_TXEN;

  NRF_PPI->CH[19].EEP = (uint32_t)&NRF_RADIO->EVENTS_DISABLED;
  NRF_PPI->CH[19].TEP = (uint32_t)&NRF_CLOCK->TASKS_HFCLKSTOP;

  NVIC_EnableIRQ(RADIO_IRQn);
  return 0;
}

int radio_start() {
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  return 0;
}

int radio_cb_register(radio_evt_t evt, RADIO_CALLBACK cb) {
  switch (evt) {
    case RADIO_EVT_DISABLED:
      cb_disabled = cb;
      NRF_RADIO->INTENSET = RADIO_INTENSET_DISABLED_Msk;
      break;
    case RADIO_EVT_READY:
      cb_ready = cb;
      NRF_RADIO->INTENSET = RADIO_INTENSET_READY_Msk;
      break;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = cb;
      NRF_RADIO->INTENSET = RADIO_INTENSET_CRCOK_Msk;
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
    case RADIO_EVT_READY:
      cb_ready = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_READY_Msk;
      break;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_CRCOK_Msk;
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
  if (NRF_RADIO->EVENTS_READY == 1) {
    NRF_RADIO->EVENTS_READY = 0;
    if (cb_ready != NULL)
      cb_ready();
  }
  if (NRF_RADIO->EVENTS_CRCOK == 1) {
    NRF_RADIO->EVENTS_CRCOK = 0;
    if (cb_crcok != NULL)
      cb_crcok();
  }
}
