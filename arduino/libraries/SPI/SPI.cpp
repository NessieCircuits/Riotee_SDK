#include "riotee.h"
#include "riotee_spic.h"
#include "printf.h"

#include "SPI.h"

#define RX_BUF_SIZE 256
static uint8_t rx_buf[RX_BUF_SIZE];

void RioteeSPI::beginTransaction(void) {
  beginTransaction(arduino::DEFAULT_SPI_SETTINGS);
}

void RioteeSPI::beginTransaction(arduino::SPISettings settings, unsigned int pin_cs, unsigned int pin_sck,
                                 unsigned int pin_copi, unsigned int pin_cipo) {
  riotee_spic_cfg_t spic_cfg;
  unsigned int f_clk = settings.getClockFreq();
  if (f_clk > 32000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M32;
  else if (f_clk > 16000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M16;
  else if (f_clk > 8000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M8;
  else if (f_clk > 4000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M4;
  else if (f_clk > 2000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M2;
  else if (f_clk > 1000000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_M1;
  else if (f_clk > 500000)
    spic_cfg.frequency = RIOTEE_SPIC_FREQUENCY_K500;
  else if (f_clk > 250000)
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

  spic_cfg.pin_cs = pin_cs;
  spic_cfg.pin_sck = pin_sck;
  spic_cfg.pin_cipo = pin_cipo;
  spic_cfg.pin_copi = pin_copi;

  riotee_spic_init(&spic_cfg);
  printf("INIT OK");
}

void RioteeSPI::beginTransaction(arduino::SPISettings settings, unsigned int pin_sck, unsigned int pin_copi,
                                 unsigned int pin_cipo) {
  beginTransaction(settings, RIOTEE_SPIC_PIN_UNUSED, pin_sck, pin_copi, pin_cipo);
}

void RioteeSPI::beginTransaction(arduino::SPISettings settings) {
  beginTransaction(settings, RIOTEE_SPIC_PIN_UNUSED, PIN_D8, PIN_D10, PIN_D9);
}

uint8_t RioteeSPI::transfer(uint8_t data) {
  uint8_t rx_buf;
  riotee_spic_transfer(&data, 1, &rx_buf, 1);
  return rx_buf;
}

uint16_t RioteeSPI::transfer16(uint16_t data) {
  uint16_t rx_buf;
  riotee_spic_transfer((uint8_t *)&data, 2, (uint8_t *)&rx_buf, 2);
  return rx_buf;
}

void RioteeSPI::transfer(void *buf, size_t count) {
  size_t rx_count = count;
  if (rx_count > RX_BUF_SIZE)
    rx_count = RX_BUF_SIZE;
  riotee_spic_transfer((uint8_t *)buf, count, rx_buf, rx_count);
  memcpy(buf, rx_buf, rx_count);
}

RioteeSPI SPI;