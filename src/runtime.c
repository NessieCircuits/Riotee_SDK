#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "timing.h"
#include "gpint.h"
#include "printf.h"
#include "riotee_module_pins.h"
#include "uart.h"
#include "nvm.h"
#include "runtime.h"

#define USR_STACK_SIZE (configMINIMAL_STACK_SIZE + 2048)
#define SYS_STACK_SIZE (configMINIMAL_STACK_SIZE + 2048)

extern unsigned long __etext;
extern unsigned long __bss_retained_start__;
extern unsigned long __bss_retained_end__;

extern unsigned long __data_retained_start__;
extern unsigned long __data_retained_end__;
extern unsigned long __data_start__;

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

static bool check_fresh_start() {
  /* If the marker is what we expect it, this is a fresh reset */
  if (fresh_marker == 0x8BADF00D) {
    /* Overwrite the marker in flash, so next time we know that it's not a fresh start */
    return true;
  }
  return false;
}

typedef struct {
  uint32_t signature;
  uint32_t top_of_stack;
  uint32_t stack_size;
  uint32_t data_size;
  uint32_t bss_size;
} snapshot_header_t;

enum { NVM_SIG_VALID = 0x0D15EA5E, NVM_SIG_INVALID = 0x8BADF00D };

static int taskstore_write(task_store_t *task_str) {
  NRF_NVMC->CONFIG |= NVMC_CONFIG_WEN_Msk;

  snapshot_header_t hdr;

  hdr.top_of_stack = *(uint32_t *)&task_str->tcb;

  hdr.stack_size = ((unsigned int)&task_str->stack[USR_STACK_SIZE - 1] - hdr.top_of_stack) / sizeof(StackType_t);

  hdr.data_size = (unsigned int)&__data_retained_end__ - (unsigned int)&__data_retained_start__;

  hdr.bss_size = (unsigned int)&__bss_retained_end__ - (unsigned int)&__bss_retained_start__;

  /* We will write the snapshot with an invalid signature first*/
  hdr.signature = NVM_SIG_INVALID;

  nvm_start(NVM_WRITE, 0x0);
  nvm_write((uint8_t *)&hdr, sizeof(snapshot_header_t));
  nvm_write((uint8_t *)hdr.top_of_stack, hdr.stack_size * sizeof(StackType_t));
  nvm_write((uint8_t *)&__data_retained_start__, hdr.data_size);
  nvm_write((uint8_t *)&__bss_retained_start__, hdr.bss_size);

  nvm_stop();
  nrf_delay_us(25);

  /* Now that the snapshot was written, we can update the signature*/
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

static int taskstore_get(task_store_t *task_str) {
  snapshot_header_t hdr;

  nvm_start(NVM_READ, 0x0);
  nvm_read((uint8_t *)&hdr, sizeof(snapshot_header_t));

  /* Check the signature to avoid loading garbage */
  if (hdr.signature != NVM_SIG_VALID) {
    printf("E%08X", hdr.signature);
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

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* This is the critical callback that suspends the user task */
static void threshold_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (pin_no == PIN_PWRGD_L) {
    vTaskNotifyGiveIndexedFromISR(sys_task_handle, 1, &xHigherPriorityTaskWoken);
  } else if (pin_no == PIN_PWRGD_H) {
    xHigherPriorityTaskWoken = xTaskResumeFromISR(usr_task_handle);
    gpint_register(PIN_PWRGD_L, GPINT_LEVEL_LOW, threshold_callback);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  return;
}

/* This is just used once after reset */
static void initialhigh_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveIndexedFromISR(sys_task_handle, 1, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
};

/* Initializes 'retained' section */
static void initialize_retained(void) {
  volatile unsigned long *src, *dst;
  src = &__etext + (&__data_retained_start__ - &__data_start__);
  dst = &__data_retained_start__;
  while (dst < &__data_retained_end__) *(dst++) = *(src++);

  src = &__bss_retained_start__;
  while (src < &__bss_retained_end__) *(src++) = 0;
}

static void sys_task(void *pvParameter) {
  UNUSED_PARAMETER(pvParameter);

  /* Make sure that the user task does not yet start */
  vTaskSuspend(usr_task_handle);

  /* Make sure we are fully charged here */
  gpint_register(PIN_PWRGD_H, GPINT_LEVEL_HIGH, initialhigh_callback);
  ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

  if (check_fresh_start()) {
    initialize_retained();
    bootstrap_callback();
  } else {
    if (taskstore_get(&usr_task_store) == 0)
      /* Unblock the user task */
      xTaskNotifyIndexed(usr_task_handle, 1, USR_EVT_RESET, eSetValueWithOverwrite);
    else {
      initialize_retained();
      /* Call user bootstrap code */
      bootstrap_callback();
    }
  }

  reset_callback();

  vTaskResume(usr_task_handle);

  gpint_register(PIN_PWRGD_L, GPINT_LEVEL_LOW, threshold_callback);
  for (;;) {
    ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);
    vTaskSuspend(usr_task_handle);
    taskstore_write(&usr_task_store);
    gpint_register(PIN_PWRGD_H, GPINT_LEVEL_HIGH, threshold_callback);
  }
}

void runtime_start(void) {
  uart_init(PIN_GPIO5);

  gpint_init();
  timing_init();

  nvm_init();

  usr_task_handle = xTaskCreateStatic(user_task, "USR", USR_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2,
                                      usr_task_store.stack, &usr_task_store.tcb);

  sys_task_handle = xTaskCreateStatic(sys_task, "SYS", SYS_STACK_SIZE, NULL, (configMAX_PRIORITIES - 1),
                                      uxSystemTaskStack, &xSystemTaskTCB);

  vTaskStartScheduler();
}
