#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "nrf.h"
#include "FreeRTOS.h"
#include "task.h"

int runtime_init(void);
void runtime_start(void);

__attribute__((always_inline)) static inline void enter_low_power(void) {
  __WFE();
  __SEV();
  __WFE();
}

/* Lower 4 bits are reserved for individual events in each module. */
enum {
  /* Teardown may happen in addition to the module event. */
  EVT_TEARDOWN = (1UL << 31),
  /* Reset may happen in addition to the module event. */
  EVT_RESET = (1UL << 30),
  /* Can be used by external drivers. */
  EVT_DRV = (1UL << 29),
  EVT_RUNTIME_BASE = (1UL << 28),
  EVT_RTC_BASE = (1UL << 27),
  EVT_BLE_BASE = (1UL << 26),
  EVT_GPIO_BASE = (1UL << 25),
  EVT_ADC_BASE = (1UL << 24),
  EVT_SPIC_BASE = (1UL << 23),
  EVT_I2C_BASE = (1UL << 22),
  EVT_STELLA_BASE = (1UL << 21),
};

typedef struct {
  unsigned int n_reset;
  unsigned int n_suspend;
} runtime_stats_t;

extern TaskHandle_t usr_task_handle;
extern TaskHandle_t sys_task_handle;

extern runtime_stats_t runtime_stats;

#define TEARDOWN_FUN(x) void (*x)() __attribute__((section(".teardown")))

#endif /* __RUNTIME_H_ */