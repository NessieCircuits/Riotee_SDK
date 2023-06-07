#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "FreeRTOS.h"
#include "task.h"

#define UNUSED_VARIABLE(X) ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)

int runtime_init(void);
void runtime_start(void);

__attribute__((always_inline)) static inline void enter_low_power(void) {
  __WFE();
  __SEV();
  __WFE();
}

enum {
  USR_EVT_RESET = 0xF000,
  USR_EVT_RTC = 0xF001,
  USR_EVT_BLE = 0xF002,
  USR_EVT_GPINT = 0xF003,
  USR_EVT_ADC = 0xF004,
  USR_EVT_STELLA_TIMEOUT = 0xF005,
  USR_EVT_STELLA_RCVD = 0xF006,
  USR_EVT_STELLA_CRCERR = 0xF007,
  SYS_EVT_PWRGD_L = 0xA000,
  SYS_EVT_PWRGD_H = 0xA001,
  SYS_EVT_RTC = 0xA002,
};

extern TaskHandle_t usr_task_handle;
extern TaskHandle_t sys_task_handle;

#endif /* __RUNTIME_H_ */