#include "riotee_spic.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "runtime.h"
#include "riotee.h"

TEARDOWN_FUN(spic_teardown_ptr);

riotee_rc_t riotee_spic_init(riotee_spic_cfg_t* cfg) {
  NRF_SPIM3->PSEL.CSN = cfg->pin_cs;
  NRF_SPIM3->PSEL.MOSI = cfg->pin_copi;
  NRF_SPIM3->PSEL.MISO = cfg->pin_cipo;
  NRF_SPIM3->PSEL.SCK = cfg->pin_sck;

  NRF_SPIM3->FREQUENCY = cfg->frequency;
  switch (cfg->order) {
    case RIOTEE_SPIC_ORDER_MSBFIRST:
      NRF_SPIM3->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos);
      break;
    case RIOTEE_SPIC_ORDER_LSBFIRST:
      NRF_SPIM3->CONFIG = (SPIM_CONFIG_ORDER_LsbFirst << SPIM_CONFIG_ORDER_Pos);
      break;
    default:
      return RIOTEE_ERR_INVALIDARG;
  }

  switch (cfg->mode) {
    case RIOTEE_SPIC_MODE0_CPOL0_CPHA0:
      NRF_SPIM3->CONFIG |=
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      break;
    case RIOTEE_SPIC_MODE1_CPOL0_CPHA1:
      NRF_SPIM3->CONFIG |=
          (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      break;
    case RIOTEE_SPIC_MODE2_CPOL1_CPHA0:
      NRF_SPIM3->CONFIG |=
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
      break;
    case RIOTEE_SPIC_MODE3_CPOL1_CPHA1:
      NRF_SPIM3->CONFIG |=
          (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
      break;
    default:
      return RIOTEE_ERR_INVALIDARG;
  }

  __NVIC_EnableIRQ(SPIM3_IRQn);
  return RIOTEE_SUCCESS;
}

static void teardown() {
  /* Prevent interrupt when aborting SPI */
  NRF_SPIM3->TASKS_STOP = 1;
  NRF_SPIM3->INTENCLR = SPIM_INTENCLR_END_Msk;

  while (NRF_SPIM3->EVENTS_STOPPED == 0) {
  }
  NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
  xTaskNotifyIndexed(usr_task_handle, 1, EVT_TEARDOWN, eSetBits);
  spic_teardown_ptr = NULL;
}

void SPIM3_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_SPIM3->EVENTS_END == 1) {
    NRF_SPIM3->EVENTS_END = 0;
    NRF_SPIM3->TASKS_STOP = 1;
    xTaskNotifyIndexedFromISR(usr_task_handle, 1, EVT_SPIC_BASE, eSetBits, &xHigherPriorityTaskWoken);
    spic_teardown_ptr = NULL;
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

riotee_rc_t riotee_spic_transfer(uint8_t* data_tx, size_t n_tx, uint8_t* data_rx, size_t n_rx) {
  unsigned long notification_value;

  taskENTER_CRITICAL();
  NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);

  NRF_SPIM3->TXD.PTR = (uint32_t)data_tx;
  NRF_SPIM3->TXD.MAXCNT = n_tx;
  NRF_SPIM3->RXD.PTR = (uint32_t)data_rx;
  NRF_SPIM3->RXD.MAXCNT = n_rx;

  xTaskNotifyStateClearIndexed(usr_task_handle, 1);
  ulTaskNotifyValueClearIndexed(usr_task_handle, 1, 0xFFFFFFFF);

  NRF_SPIM3->EVENTS_END = 0;
  NRF_SPIM3->EVENTS_STOPPED = 0;
  NRF_SPIM3->INTENSET = SPIM_INTENSET_END_Msk;

  NRF_SPIM3->TASKS_START = 1;
  spic_teardown_ptr = teardown;
  taskEXIT_CRITICAL();

  xTaskNotifyWaitIndexed(1, 0x0, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (notification_value & EVT_RESET)
    return RIOTEE_ERR_RESET;
  if (notification_value & EVT_TEARDOWN)
    return RIOTEE_ERR_TEARDOWN;

  if (notification_value == EVT_SPIC_BASE) {
    while (NRF_SPIM3->EVENTS_STOPPED == 0) {
    }
    NRF_SPIM3->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
    return RIOTEE_SUCCESS;
  }
  return RIOTEE_ERR_GENERIC;
}

void riotee_spic_def_cfg(riotee_spic_cfg_t* cfg) {
  cfg->frequency = RIOTEE_SPIC_FREQUENCY_M8;
  cfg->mode = RIOTEE_SPIC_MODE0_CPOL0_CPHA0;
  cfg->order = RIOTEE_SPIC_ORDER_MSBFIRST;
  cfg->pin_cs = PIN_D7;
  cfg->pin_sck = PIN_D8;
  cfg->pin_copi = PIN_D10;
  cfg->pin_cipo = PIN_D9;
}