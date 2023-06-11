#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "riotee_timing.h"
#include "riotee_gpint.h"
#include "printf.h"
#include "riotee.h"
#include "riotee_uart.h"
#include "riotee_nvm.h"
#include "runtime.h"
#include "riotee_thresholds.h"

#define USR_STACK_SIZE (configMINIMAL_STACK_SIZE + 2048)
#define SYS_STACK_SIZE (configMINIMAL_STACK_SIZE + 2048)

extern unsigned long __etext;
extern unsigned long __bss_retained_start__;
extern unsigned long __bss_retained_end__;

extern unsigned long __data_retained_start__;
extern unsigned long __data_retained_end__;
extern unsigned long __data_start__;

/* Linker section where drivers register their teardown functions */
extern unsigned long __teardown_start__;
extern unsigned long __teardown_end__;

/* This marker is used to check if device has been reset before */
unsigned long fresh_marker = 0x8BADF00D;

typedef struct {
  StackType_t stack[USR_STACK_SIZE];
  StaticTask_t tcb;
} task_store_t;

/* Holds stack and task control block of the user task */
task_store_t usr_task_store;

StaticTask_t xIdleTaskTCB;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

StaticTask_t xSystemTaskTCB;
StackType_t uxSystemTaskStack[SYS_STACK_SIZE];

TaskHandle_t sys_task_handle;
TaskHandle_t usr_task_handle;

void sys_setup_timer(unsigned int ticks);
void sys_cancel_timer(void);

/* Dummy callback to be called when low capacitor voltage is detected. Can be overwritten by the user. */
__attribute__((weak)) void turnoff_callback(void){};

/* Checks if a certain value is found in flash memory to determine if this is the first boot after programming. */
static bool check_fresh_start() {
  /* If the marker is what we expect it, this is a fresh reset */
  if (fresh_marker == 0x8BADF00D) {
    return true;
  }
  return false;
}

/* Overwrites a value in flash memory that indicates if the current boot is the first boot after programming.*/
static void overwrite_marker() {
  /* Get LMA of marker. Compare to linkerscript to understand this calculation */
  unsigned long *marker_nvm_addr = &__etext + (&fresh_marker - &__data_start__);

  /* Enable write to flash */
  NRF_NVMC->CONFIG |= NVMC_CONFIG_WEN_Msk;
  /* Wait for flash to become ready */
  while (NRF_NVMC->READY == 0) __NOP();
  /* Overwrite marker with all zeroes (we can only write 0) */
  *marker_nvm_addr = 0x0;
  __DMB();
  /* Disable write to flash */
  NRF_NVMC->CONFIG &= ~NVMC_CONFIG_WEN_Msk;
}

typedef struct {
  uint32_t signature;
  uint32_t top_of_stack;
  uint32_t stack_size;
  uint32_t data_size;
  uint32_t bss_size;
} snapshot_header_t;

enum { NVM_SIG_VALID = 0x0D15EA5E, NVM_SIG_INVALID = 0x8BADF00D };

/* Stores task stack and static/global variables in non-volatile memory. */
static int checkpoint_store(task_store_t *task_str) {
  snapshot_header_t hdr;

  hdr.top_of_stack = *(uint32_t *)&task_str->tcb;

  hdr.stack_size = ((unsigned int)&task_str->stack[USR_STACK_SIZE - 1] - hdr.top_of_stack) / sizeof(StackType_t);

  hdr.data_size = (unsigned int)&__data_retained_end__ - (unsigned int)&__data_retained_start__;

  hdr.bss_size = (unsigned int)&__bss_retained_end__ - (unsigned int)&__bss_retained_start__;

  /* We will write the snapshot with an invalid signature as the first value. This makes sure that no faulty snapshot is
   * loaded. */
  hdr.signature = NVM_SIG_INVALID;

  nvm_start(NVM_WRITE, 0x0);
  nvm_write((uint8_t *)&hdr, sizeof(snapshot_header_t));
  nvm_write((uint8_t *)hdr.top_of_stack, hdr.stack_size * sizeof(StackType_t));
  nvm_write((uint8_t *)&__data_retained_start__, hdr.data_size);
  nvm_write((uint8_t *)&__bss_retained_start__, hdr.bss_size);

  nvm_stop();

  /* Now that the snapshot was written successfully, we can update the signature */
  nvm_start(NVM_WRITE, 0x0);
  uint32_t signature = NVM_SIG_VALID;
  nvm_write((uint8_t *)&signature, sizeof(signature));
  nvm_stop();

  /* If this was a first boot, overwrite the marker now */
  if (check_fresh_start()) {
    overwrite_marker();
  }

  return 0;
}

/* Loads a snapshot from NVM into task stack and static/global variables. */
static int checkpoint_load(task_store_t *task_str) {
  snapshot_header_t hdr;

  nvm_start(NVM_READ, 0x0);
  nvm_read((uint8_t *)&hdr, sizeof(snapshot_header_t));

  /* Check the signature to avoid loading garbage */
  if (hdr.signature != NVM_SIG_VALID) {
    nvm_stop();
    return -1;
  }

  /* Restore stack */
  nvm_read((uint8_t *)hdr.top_of_stack, hdr.stack_size * sizeof(StackType_t));
  /* Restore static/global variables */
  nvm_read((uint8_t *)&__data_retained_start__, hdr.data_size);
  nvm_read((uint8_t *)&__bss_retained_start__, hdr.bss_size);
  nvm_stop();

  /* Copy top of stack into freertos TCB structure */
  memcpy(&task_str->tcb, &hdr.top_of_stack, sizeof(uint32_t));
  return 0;
}

/* We are not using any timer for scheduling */
void vPortSetupTimerInterrupt(void) {
  return;
}

/* This keeps the MCU in low power mode */
void vApplicationIdleHook(void) {
  enter_low_power();
  return;
}

/* Assigns statically allocated memory for FreeRTOS idle task */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static void threshold_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (pin_no == PIN_PWRGD_L) {
    xTaskNotifyIndexedFromISR(sys_task_handle, 1, EVT_PWRGD_L, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  } else if (pin_no == PIN_PWRGD_H) {
    xTaskNotifyIndexedFromISR(sys_task_handle, 1, EVT_PWRGD_H, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  return;
}

/* Initializes 'retained' section when no checkpoint exists. */
static void initialize_retained(void) {
  volatile unsigned long *src, *dst;
  src = &__etext + (&__data_retained_start__ - &__data_start__);
  dst = &__data_retained_start__;
  while (dst < &__data_retained_end__) *(dst++) = *(src++);

  src = &__bss_retained_start__;
  while (src < &__bss_retained_end__) *(src++) = 0;
}

/* Waits until capacitor is fully charged as indicated by PWRGD_H pin */
int wait_until_charged(void) {
  return riotee_gpint_wait(PIN_PWRGD_H, GPINT_LEVEL_HIGH, GPIO_PIN_CNF_PULL_Disabled);
}

static void teardown(void) {
  /* Call all registered teardown functions */
  void (*fn_teardown)(void);
  uint32_t *fn_addr;
  for (fn_addr = &__teardown_start__; fn_addr < &__teardown_end__; fn_addr++) {
    fn_teardown = (void (*)(void)) * fn_addr;
    if (fn_teardown != NULL)
      fn_teardown();
  }

  /* Give the application an opportunity to switch off power-hungry devices */
  turnoff_callback();
}

/* High priority system task initializes runtime, and handles intermittent execution and checkpointing. */
static void sys_task(void *pvParameter) {
  UNUSED_PARAMETER(pvParameter);

  unsigned long notification_value;

  /* Make sure that the user task does not yet start */
  vTaskSuspend(usr_task_handle);

  riotee_gpint_register(PIN_PWRGD_H, GPINT_LEVEL_HIGH, GPIO_PIN_CNF_PULL_Disabled, threshold_callback);
  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

  if (check_fresh_start()) {
    initialize_retained();
    bootstrap_callback();
  } else {
    if (checkpoint_load(&usr_task_store) == 0)
      /* Unblock the user task */
      xTaskNotifyIndexed(usr_task_handle, 1, EVT_RESET, eSetValueWithOverwrite);
    else {
      initialize_retained();
      /* Call user bootstrap code */
      bootstrap_callback();
    }
  }

  reset_callback();

  for (;;) {
    vTaskResume(usr_task_handle);

    /* Wait until capacitor voltage falls below the 'low' threshold */
    riotee_gpint_register(PIN_PWRGD_L, GPINT_LEVEL_LOW, GPIO_PIN_CNF_PULL_Disabled, threshold_callback);
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

    teardown();
    vTaskSuspend(usr_task_handle);

    /* Set a high threshold - upon reaching this threshold, execution continues */
    /* If the user task was already waiting on high threshold, we have to notify it here */
    if (riotee_gpint_unregister(PIN_PWRGD_H) == GPINT_ERR_OK)
      xTaskNotifyIndexed(usr_task_handle, 1, EVT_GPINT, eSetValueWithOverwrite);
    riotee_gpint_register(PIN_PWRGD_H, GPINT_LEVEL_HIGH, GPIO_PIN_CNF_PULL_Disabled, threshold_callback);

    /* Set a 10ms timer*/
    sys_setup_timer(8333);
    /* Wait until capacitor is recharged or timer expires */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
    /* Recharged? */
    if (notification_value == EVT_PWRGD_H) {
      sys_cancel_timer();
      xTaskNotifyStateClearIndexed(xTaskGetCurrentTaskHandle(), 1);
      continue;
    }

    /* Timer has expired. Is capacitor voltage still below threshold? */
    if ((NRF_P0->IN & (1 << PIN_PWRGD_L)) == 0) {
      /* Take the snapshot */
      checkpoint_store(&usr_task_store);
    } else {
      /* Monitor for capacitor voltage to drop below threshold again */
      riotee_gpint_register(PIN_PWRGD_L, GPINT_LEVEL_LOW, GPIO_PIN_CNF_PULL_Disabled, threshold_callback);
    }

    /* Wait until capacitor is recharged or discharged below the crtitical threshold again */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
    /* Recharged? */
    if (notification_value == EVT_PWRGD_H) {
      riotee_gpint_unregister(PIN_PWRGD_L);
      continue;
    }
    /* Dropped below the threshold again -> take a snapshot */
    checkpoint_store(&usr_task_store);
    /* Wait until capacitor is recharged */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  }
}

void runtime_start(void) {
  riotee_uart_init(PIN_D1, 1000000UL);

  riotee_gpint_init();
  riotee_timing_init();

  nvm_init();

  usr_task_handle = xTaskCreateStatic(user_task, "USR", USR_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2,
                                      usr_task_store.stack, &usr_task_store.tcb);

  sys_task_handle = xTaskCreateStatic(sys_task, "SYS", SYS_STACK_SIZE, NULL, (configMAX_PRIORITIES - 1),
                                      uxSystemTaskStack, &xSystemTaskTCB);

  vTaskStartScheduler();
}
