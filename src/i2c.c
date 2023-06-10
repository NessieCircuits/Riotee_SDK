#include <stdlib.h>
#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"

#include "riotee.h"
#include "riotee_i2c.h"
#include "printf.h"
#include "runtime.h"

static volatile enum { TWI_STAT_BUSY = 1, TWI_STAT_ERROR = -1, TWI_STAT_STOPPED = 0 } twi_status;

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
  if (NRF_TWIM1->EVENTS_ERROR == 1) {
    NRF_TWIM1->EVENTS_ERROR = 0;
    NRF_TWIM1->TASKS_STOP = 1;
    twi_status = TWI_STAT_ERROR;
  } else if (NRF_TWIM1->EVENTS_STOPPED == 1) {
    NRF_TWIM1->EVENTS_STOPPED = 0;
    twi_status = TWI_STAT_STOPPED;
  }
}

int i2c_init(unsigned int pinSDA, unsigned int pinSCL) {
  NRF_TWIM1->PSEL.SCL = pinSCL;
  NRF_TWIM1->PSEL.SDA = pinSDA;

  /* These should stay high when I2C is disabled */
  nrf_gpio_cfg_input(PIN_SYS_SCL, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(PIN_SYS_SDA, NRF_GPIO_PIN_PULLUP);

  NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K250 << TWIM_FREQUENCY_FREQUENCY_Pos;
  NRF_TWIM1->SHORTS = 0;

  NRF_TWIM1->INTENSET = TWIM_INTENSET_ERROR_Msk | TWIM_INTENSET_STOPPED_Msk;
  NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
  return 0;
}

int i2c_write(uint8_t dev_addr, uint8_t *data, size_t n_data) {
  taskENTER_CRITICAL();

  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->TXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.MAXCNT = 0;
  NRF_TWIM1->TXD.PTR = (uint32_t)data;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;

  twi_status = TWI_STAT_BUSY;
  NRF_TWIM1->TASKS_STARTTX = 1;
  while (twi_status == TWI_STAT_BUSY) {
    enter_low_power();
  }
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  taskEXIT_CRITICAL();

  return twi_status;
}

int i2c_read(uint8_t *buffer, size_t n_data, uint8_t dev_addr) {
  taskENTER_CRITICAL();
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;

  NRF_TWIM1->TXD.MAXCNT = 0;
  NRF_TWIM1->RXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.PTR = (uint32_t)buffer;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;
  twi_status = TWI_STAT_BUSY;
  NRF_TWIM1->TASKS_STARTRX = 1;
  while (twi_status == TWI_STAT_BUSY) {
    enter_low_power();
  }
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  taskEXIT_CRITICAL();

  return twi_status;
}