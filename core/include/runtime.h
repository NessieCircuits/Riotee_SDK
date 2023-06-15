#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "FreeRTOS.h"
#include "task.h"

#define UNUSED_VARIABLE(X) ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)

#define USR_STACK_SIZE_WORDS (1024)

int runtime_init(void);
void runtime_start(void);

__attribute__((always_inline)) static inline void enter_low_power(void) {
  __WFE();
  __SEV();
  __WFE();
}

enum {
  EVT_TEARDOWN = (1UL << 31),
  EVT_RESET = (1UL << 30),
  EVT_RTC = (1UL << 0),
  EVT_BLE = (1UL << 1),
  EVT_GPINT = (1UL << 2),
  EVT_ADC = (1UL << 3),
  EVT_SPIC = (1UL << 4),
  EVT_STELLA_TIMEOUT = (1UL << 5),
  EVT_STELLA_RCVD = (1UL << 6),
  EVT_STELLA_CRCERR = (1UL << 7),
  EVT_PWRGD_L = 0xA000,
  EVT_PWRGD_H = 0xA001,
};

typedef struct {
  unsigned int n_reset;
  unsigned int n_turnoff;
} runtime_stats_t;

extern TaskHandle_t usr_task_handle;
extern TaskHandle_t sys_task_handle;

extern runtime_stats_t runtime_stats;

#define TEARDOWN_FUN(x) void (*x)() __attribute__((section(".teardown")))

#endif /* __RUNTIME_H_ */