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
  SYS_EVT_PWRGD_L = 0xA000,
  SYS_EVT_PWRGD_H = 0xA001,
  SYS_EVT_RTC = 0xA002,
};

extern TaskHandle_t usr_task_handle;
extern TaskHandle_t sys_task_handle;

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

/* Waits until capacitor is fully charged as indicated by PWRGD_H pin */
int wait_until_charged(void);

#define __VOLATILE_INITIALIZED __attribute__((section(".volatile.data")))
#define __VOLATILE_UNINITIALIZED __attribute__((section(".volatile.bss")))

#if defined __cplusplus
}
#endif

#endif /* __RUNTIME_H_ */