#include "nrf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "runtime.h"

/* This points to the task currently blocking on GPINT event */
static TaskHandle_t waiting_task;

void RTC0_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (NRF_RTC0->EVENTS_COMPARE[0] == 1) {
    NRF_RTC0->EVENTS_COMPARE[0] = 0;

    xTaskNotifyIndexedFromISR(waiting_task, 1, USR_EVT_RTC, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void delay(unsigned int ms) {
  unsigned long notification_value;
  waiting_task = xTaskGetCurrentTaskHandle();
  NRF_RTC0->CC[0] = (NRF_RTC0->COUNTER + ms) % (1 << 24);
  NRF_RTC0->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
}

int timing_init(void) {
  NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal;

  NRF_CLOCK->TASKS_LFCLKSTART = 1;

  NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;
  NVIC_EnableIRQ(RTC0_IRQn);

  NRF_RTC0->PRESCALER = 31;
  NRF_RTC0->TASKS_CLEAR = 1;
  NRF_RTC0->TASKS_START = 1;
  return 0;
}