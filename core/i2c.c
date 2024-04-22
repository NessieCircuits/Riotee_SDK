#include <stdlib.h>
#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "riotee.h"
#include "riotee_i2c.h"
#include "printf.h"
#include "runtime.h"

TEARDOWN_FUN(i2c_teardown_ptr);

static TaskHandle_t blocking_task;

enum {
  EVT_I2C_STOPPED = EVT_I2C_BASE + 0,
  EVT_I2C_ERROR = EVT_I2C_BASE + 1,
};

static void teardown(void) {
  NRF_TWIM1->TASKS_STOP = 1;
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;

  xTaskNotifyIndexed(blocking_task, 1, EVT_TEARDOWN, eSetBits);
  i2c_teardown_ptr = NULL;
}

void riotee_i2c_set_freq(riotee_i2c_freq_t freq) {
  NRF_TWIM1->FREQUENCY = freq;
}

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_TWIM1->EVENTS_ERROR == 1) {
    NRF_TWIM1->EVENTS_ERROR = 0;
    NRF_TWIM1->TASKS_STOP = 1;
    xTaskNotifyIndexedFromISR(blocking_task, 1, EVT_I2C_ERROR, eSetBits, &xHigherPriorityTaskWoken);

  } else if (NRF_TWIM1->EVENTS_STOPPED == 1) {
    NRF_TWIM1->EVENTS_STOPPED = 0;
    xTaskNotifyIndexedFromISR(blocking_task, 1, EVT_I2C_STOPPED, eSetBits, &xHigherPriorityTaskWoken);
  }
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  i2c_teardown_ptr = NULL;
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void i2c_init(unsigned int pin_sda, unsigned int pin_scl) {
  NRF_TWIM1->PSEL.SCL = pin_scl;
  NRF_TWIM1->PSEL.SDA = pin_sda;

  /* These should stay high when I2C is disabled */
  nrf_gpio_cfg_input(pin_scl, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(pin_sda, NRF_GPIO_PIN_PULLUP);

  riotee_i2c_set_freq(RIOTEE_I2C_FREQ_250K);
  NRF_TWIM1->SHORTS = 0;

  NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
}

void riotee_i2c_init(void) {
  i2c_init(PIN_SYS_SDA, PIN_SYS_SCL);
}

riotee_rc_t riotee_i2c_write(uint8_t dev_addr, uint8_t *data, size_t n_data) {
  unsigned long notification_value;

  taskENTER_CRITICAL();

  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->TXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.MAXCNT = 0;
  NRF_TWIM1->TXD.PTR = (uint32_t)data;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;

  NRF_TWIM1->INTENSET = TWIM_INTENSET_ERROR_Msk | TWIM_INTENSET_STOPPED_Msk;

  blocking_task = xTaskGetCurrentTaskHandle();
  xTaskNotifyStateClearIndexed(blocking_task, 1);
  ulTaskNotifyValueClearIndexed(blocking_task, 1, 0xFFFFFFFF);
  i2c_teardown_ptr = teardown;
  NRF_TWIM1->TASKS_STARTTX = 1;

  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value == EVT_I2C_ERROR)
    return RIOTEE_ERR_COMMI2C;
  if (notification_value == EVT_I2C_STOPPED)
    return RIOTEE_SUCCESS;
  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;
  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;

  return RIOTEE_ERR_GENERIC;
}

riotee_rc_t riotee_i2c_read(uint8_t *buffer, size_t n_data, uint8_t dev_addr) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;

  NRF_TWIM1->TXD.MAXCNT = 0;
  NRF_TWIM1->RXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.PTR = (uint32_t)buffer;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;

  NRF_TWIM1->INTENSET = TWIM_INTENSET_ERROR_Msk | TWIM_INTENSET_STOPPED_Msk;

  blocking_task = xTaskGetCurrentTaskHandle();
  xTaskNotifyStateClearIndexed(blocking_task, 1);
  ulTaskNotifyValueClearIndexed(blocking_task, 1, 0xFFFFFFFF);
  i2c_teardown_ptr = teardown;

  NRF_TWIM1->TASKS_STARTRX = 1;

  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value == EVT_I2C_ERROR)
    return RIOTEE_ERR_COMMI2C;
  if (notification_value == EVT_I2C_STOPPED)
    return RIOTEE_SUCCESS;
  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;
  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;

  return RIOTEE_ERR_GENERIC;
}

riotee_rc_t riotee_i2c_write_atomic(uint8_t dev_addr, uint8_t *data, size_t n_data) {
  taskENTER_CRITICAL();

  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->TXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.MAXCNT = 0;
  NRF_TWIM1->TXD.PTR = (uint32_t)data;

  /* Disable all interrupts. */
  NRF_TWIM1->INTENCLR = 0xFFFFFFFF;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;

  NRF_TWIM1->TASKS_STARTTX = 1;
  riotee_rc_t rc;
  while (1) {
    if (NRF_TWIM1->EVENTS_STOPPED == 1) {
      NRF_TWIM1->EVENTS_STOPPED = 0;
      rc = RIOTEE_SUCCESS;
      break;
    }
    if (NRF_TWIM1->EVENTS_ERROR == 1) {
      NRF_TWIM1->EVENTS_ERROR = 0;
      rc = RIOTEE_ERR_COMMI2C;
      break;
    }
  }
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  taskEXIT_CRITICAL();
  return rc;
}

riotee_rc_t riotee_i2c_read_atomic(uint8_t *buffer, size_t n_data, uint8_t dev_addr) {
  taskENTER_CRITICAL();
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;

  NRF_TWIM1->ADDRESS = dev_addr;

  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;

  NRF_TWIM1->TXD.MAXCNT = 0;
  NRF_TWIM1->RXD.MAXCNT = n_data;
  NRF_TWIM1->RXD.PTR = (uint32_t)buffer;

  /* Disable all interrupts. */
  NRF_TWIM1->INTENCLR = 0xFFFFFFFF;

  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;

  NRF_TWIM1->TASKS_STARTRX = 1;

  riotee_rc_t rc;
  while (1) {
    if (NRF_TWIM1->EVENTS_STOPPED == 1) {
      NRF_TWIM1->EVENTS_STOPPED = 0;
      rc = RIOTEE_SUCCESS;
      break;
    }
    if (NRF_TWIM1->EVENTS_ERROR == 1) {
      NRF_TWIM1->EVENTS_ERROR = 0;
      rc = RIOTEE_ERR_COMMI2C;
      break;
    }
  }
  NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  taskEXIT_CRITICAL();
  return rc;
}
