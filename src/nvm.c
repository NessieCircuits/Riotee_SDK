#include "nrf.h"
#include "nrf_gpio.h"

#include "printf.h"

#include "nvm.h"
#include "riotee_module_pins.h"
#include "runtime.h"

static volatile bool nvm_event = false;
static unsigned int _pin_cs;

int nvm_init(void) {
  NRF_SPIM0->PSEL.SCK = PIN_C2C_CLK;
  NRF_SPIM0->PSEL.MOSI = PIN_C2C_MOSI;
  NRF_SPIM0->PSEL.MISO = PIN_C2C_MISO;
  NRF_SPIM0->PSEL.CSN = 0xFFFFFFFF;

  _pin_cs = PIN_C2C_CS;

  nrf_gpio_cfg_output(_pin_cs);
  nrf_gpio_pin_set(_pin_cs);

  NRF_SPIM0->CONFIG = (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) |
                      (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos) |
                      (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos);

  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8;

  __NVIC_EnableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);

  NRF_TIMER4->PRESCALER = 4;
  NRF_TIMER4->CC[0] = 15;
  NRF_TIMER4->CC[1] = 30;

  NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER4->EVENTS_COMPARE[0];
  NRF_PPI->CH[0].TEP = (uint32_t)&NRF_SPIM0->TASKS_START;

  NRF_TIMER4->SHORTS = TIMER_SHORTS_COMPARE1_STOP_Msk;

  NRF_PPI->CHENSET = PPI_CHENSET_CH0_Msk;

  NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE1_Msk;

  __NVIC_EnableIRQ(TIMER4_IRQn);
  return 0;
}

void TIMER4_IRQHandler(void) {
  if (NRF_TIMER4->EVENTS_COMPARE[1] == 1) {
    NRF_TIMER4->EVENTS_COMPARE[1] = 0;
    NRF_SPIM0->TASKS_STOP = 1;
    nvm_event = true;
  }
}

void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) {
  if (NRF_SPIM0->EVENTS_END == 1) {
    NRF_SPIM0->EVENTS_END = 0;
    NRF_SPIM0->TASKS_STOP = 1;
    nvm_event = true;
  }
}

static int _prep_xfer(uint8_t* tx_buf, uint8_t* rx_buf, size_t n_tx, size_t n_rx) {
  /* The SPI transaction will last for max(n_tx, n_rx) bytes */
  NRF_SPIM0->TXD.MAXCNT = n_tx;
  NRF_SPIM0->RXD.MAXCNT = n_rx;

  NRF_SPIM0->TXD.PTR = (uint32_t)tx_buf;
  NRF_SPIM0->RXD.PTR = (uint32_t)rx_buf;
  NRF_SPIM0->EVENTS_END = 0;
  NRF_SPIM0->EVENTS_STOPPED = 0;
  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);

  return 0;
}

int nvm_start(nvm_transfer_type_t transfer_type, uint32_t address) {
  nrf_gpio_pin_clear(_pin_cs);

  uint32_t cmd = (address & 0xFFFFF) | transfer_type;
  _prep_xfer((uint8_t*)&cmd, NULL, 3, 0);

  nvm_event = false;
  NRF_TIMER4->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_START = 1;

  while (nvm_event == false) {
    enter_low_power();
  }

  while (NRF_SPIM0->EVENTS_STOPPED == 0) {
  }

  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
  NRF_SPIM0->INTENSET = SPIM_INTENSET_END_Msk;
  /* See nRF52833 errata [78] */
  NRF_TIMER4->TASKS_SHUTDOWN = 1;

  return 0;
}

int nvm_stop(void) {
  nrf_gpio_pin_set(_pin_cs);
  NRF_SPIM0->INTENCLR = SPIM_INTENSET_END_Msk;

  return 0;
}

int nvm_write(uint8_t* src, size_t size) {
  _prep_xfer(src, NULL, size, 0);

  nvm_event = false;
  NRF_SPIM0->TASKS_START = 1;
  while (nvm_event == false) {
    enter_low_power();
  }

  while (NRF_SPIM0->EVENTS_STOPPED == 0) {
  }
  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);

  return 0;
}

int nvm_read(uint8_t* dst, size_t size) {
  _prep_xfer(NULL, dst, 0, size);

  nvm_event = false;
  NRF_SPIM0->TASKS_START = 1;
  while (nvm_event == false) {
    enter_low_power();
  }

  while (NRF_SPIM0->EVENTS_STOPPED == 0) {
  }
  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);

  return 0;
}