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

enum { USR_EVT_RESET = 0xF000, USR_EVT_RTC = 0xF001, USR_EVT_BLE = 0xF002, USR_EVT_GPINT = 0xF003 };

extern TaskHandle_t usr_task_handle;

#if defined __cplusplus
extern "C" {
#endif

/* This gets called right after startup */
void startup_callback(void);
/* This gets called one time after flashing new firmware */
void bootstrap_callback(void);
/* This gets called after every reset */
void reset_callback(void);
/* This gets called right before user code is suspended */
void turnoff_callback(void);
/* This is the FreeRTOS task running user code*/
void user_task(void *pvParameter);

#if defined __cplusplus
}
#endif

#endif /* __RUNTIME_H_ */