#include "riotee_spic.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "runtime.h"
#include "riotee.h"

TEARDOWN_FUN(spic_teardown_ptr);

int spic_init(riotee_spic_cfg_t* cfg) {
  NRF_SPIM3->PSEL.CSN = PIN_D7;
  NRF_SPIM3->PSEL.MOSI = PIN_D8;
  NRF_SPIM3->PSEL.MISO = PIN_D9;
  NRF_SPIM3->PSEL.SCK = PIN_D10;

  NRF_SPIM3->FREQUENCY = cfg->frequency;
  switch (cfg->mode) {
    case SPIC_MODE0_CPOL0_CPHA0:
      NRF_SPIM3->CONFIG =
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      break;
    case SPIC_MODE1_CPOL0_CPHA1:
      NRF_SPIM3->CONFIG =
          (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      break;
    case SPIC_MODE2_CPOL1_CPHA0:
      NRF_SPIM3->CONFIG =
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
      break;
    case SPIC_MODE3_CPOL1_CPHA1:
      NRF_SPIM3->CONFIG =
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
      break;
  }

  __NVIC_EnableIRQ(SPIM3_IRQn);
  return 0;
}

static void teardown() {
  /* Prevent interrupt when aborting SPI */
  NRF_SPIM3->TASKS_STOP = 1;
  NRF_SPIM3->INTENCLR = SPIM_INTENCLR_END_Msk;

  while (NRF_SPIM3->EVENTS_STOPPED == 0) {
  }
  NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
  xTaskNotifyIndexed(usr_task_handle, 1, EVT_TEARDOWN, eSetValueWithOverwrite);
  spic_teardown_ptr = NULL;
}

void SPIM3_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_SPIM3->EVENTS_END == 1) {
    NRF_SPIM3->EVENTS_END = 0;
    NRF_SPIM3->TASKS_STOP = 1;
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_SPIC, eSetBits, &xHigherPriorityTaskWoken);
    spic_teardown_ptr = NULL;
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int spic_transfer(uint8_t* data_tx, size_t n_tx, uint8_t* data_rx, size_t n_rx) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);

  NRF_SPIM3->TXD.PTR = (uint32_t)data_tx;
  NRF_SPIM3->TXD.MAXCNT = n_tx;
  NRF_SPIM3->RXD.PTR = (uint32_t)data_rx;
  NRF_SPIM3->RXD.MAXCNT = n_rx;

  xTaskNotifyStateClearIndexed(usr_task_handle, 1);

  NRF_SPIM3->EVENTS_END = 0;
  NRF_SPIM3->EVENTS_STOPPED = 0;
  NRF_SPIM3->INTENSET = SPIM_INTENSET_END_Msk;

  NRF_SPIM3->TASKS_START = 1;
  spic_teardown_ptr = teardown;
  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value != EVT_SPIC)
    return -1;

  while (NRF_SPIM3->EVENTS_STOPPED == 0) {
  }
  NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);

  return 0;
}
