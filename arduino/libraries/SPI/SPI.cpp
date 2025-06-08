#include "riotee.h"
#include "riotee_spic.h"
#include "riotee_gpio.h"
#include "printf.h"

#include "SPI.h"

#define RX_BUF_SIZE 256
static uint8_t rx_buf[RX_BUF_SIZE];

RioteeSPI::RioteeSPI() : _pin_copi(PIN_D10), _pin_cipo(PIN_D9), _pin_sck(PIN_D8), _pin_cs(RIOTEE_SPIC_PIN_UNUSED) {}

void RioteeSPI::begin() {
  riotee_gpio_cfg_output(_pin_copi);
  riotee_gpio_cfg_output(_pin_sck);
  riotee_gpio_cfg_input(_pin_cipo, RIOTEE_GPIO_IN_NOPULL);
  if (_pin_cs != RIOTEE_SPIC_PIN_UNUSED) {
    riotee_gpio_cfg_output(_pin_cs);
    riotee_gpio_set(_pin_cs);
  }
}

void RioteeSPI::begin(unsigned int pin_cipo, unsigned int pin_sck, unsigned int pin_copi, unsigned int pin_cs) {
  _pin_cipo = pin_cipo;
  _pin_copi = pin_copi;
  _pin_sck = pin_sck;
  _pin_cs = pin_cs;
  begin();
}

void RioteeSPI::begin(unsigned int pin_cipo, unsigned int pin_sck, unsigned int pin_copi) {
  _pin_cipo = pin_cipo;
  _pin_copi = pin_copi;
  _pin_sck = pin_sck;
  begin();
}

void RioteeSPI::end() {
  riotee_gpio_cfg_disable(_pin_copi);
  riotee_gpio_cfg_disable(_pin_sck);
  riotee_gpio_cfg_disable(_pin_cipo);
  if (_pin_cs != RIOTEE_SPIC_PIN_UNUSED) {
    riotee_gpio_cfg_disable(_pin_cs);
  }
}

void RioteeSPI::beginTransaction(arduino::SPISettings settings) {
  riotee_spic_cfg_t spic_cfg;
  unsigned int f_clk = settings.getClockFreq();
  if (f_clk >= 32000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M32;
  else if (f_clk >= 16000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M16;
  else if (f_clk >= 8000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M8;
  else if (f_clk >= 4000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M4;
  else if (f_clk >= 2000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M2;
  else if (f_clk >= 1000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M1;
  else if (f_clk >= 500000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_K500;
  else if (f_clk >= 250000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_K250;
  else
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_K125;

  switch (settings.getDataMode()) {
    case arduino::SPI_MODE0:
      spic_cfg.mode = RIOTEE_SPIC_MODE0_CPOL0_CPHA0;
      break;
    case arduino::SPI_MODE1:
      spic_cfg.mode = RIOTEE_SPIC_MODE1_CPOL0_CPHA1;
      break;
    case arduino::SPI_MODE2:
      spic_cfg.mode = RIOTEE_SPIC_MODE2_CPOL1_CPHA0;
      break;
    case arduino::SPI_MODE3:
      spic_cfg.mode = RIOTEE_SPIC_MODE3_CPOL1_CPHA1;
      break;
    default:
      /* This should not happen. */
      return;
  }

  switch (settings.getBitOrder()) {
    case MSBFIRST:
      spic_cfg.order = RIOTEE_SPIC_ORDER_MSBFIRST;
      break;
    case LSBFIRST:
      spic_cfg.order = RIOTEE_SPIC_ORDER_LSBFIRST;
      break;
    default:
      /* This should not happen. */
      return;
  }

  spic_cfg.pin_cs = _pin_cs;
  spic_cfg.pin_sck = _pin_sck;
  spic_cfg.pin_cipo = _pin_cipo;
  spic_cfg.pin_copi = _pin_copi;

  riotee_spic_init(&spic_cfg);
}

void RioteeSPI::beginTransaction(void) {
  beginTransaction(arduino::DEFAULT_SPI_SETTINGS);
}

uint8_t RioteeSPI::transfer(uint8_t data) {
  uint8_t _rx_buf;
  riotee_spic_transfer(&data, 1, &_rx_buf, 1);
  return _rx_buf;
}

uint16_t RioteeSPI::transfer16(uint16_t data) {
  uint16_t _rx_buf;
  riotee_spic_transfer((uint8_t *)&data, 2, (uint8_t *)&_rx_buf, 2);
  return _rx_buf;
}

void RioteeSPI::transfer(void *buf, size_t count) {
  size_t rx_count = count;
  if (rx_count > RX_BUF_SIZE)
    rx_count = RX_BUF_SIZE;
  riotee_spic_transfer((uint8_t *)buf, count, rx_buf, rx_count);
  memcpy(buf, rx_buf, rx_count);
}

RioteeSPI SPI;