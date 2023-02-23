#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "FreeRTOS.h"
#include "task.h"

int runtime_init(void);
void runtime_start(void);

__attribute__((always_inline)) static inline void enter_low_power(void) {
  __WFE();
  __SEV();
  __WFE();
}

enum { USR_EVT_RESET = 0xF000, USR_EVT_RTC = 0xF001, USR_EVT_BLE = 0xF002 };

extern TaskHandle_t usr_task_handle;

/* This gets called one time after flashing new firmware */
void bootstrap(void);
/* This gets called after every reset */
void setup(void);
/* This is the FreeRTOS task running user code*/
void user_task(void *pvParameter);

#define UNUSED_VARIABLE(X) ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)

#endif /* __RUNTIME_H_ */