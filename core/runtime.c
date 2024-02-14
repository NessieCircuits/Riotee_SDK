#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "riotee_timing.h"
#include "gpint.h"
#include "riotee_gpio.h"
#include "printf.h"
#include "riotee.h"
#include "riotee_uart.h"
#include "riotee_nvm.h"
#include "runtime.h"
#include "riotee_thresholds.h"

/* RIOTEE_STACK_SIZE is defined and passed in via the Makefile */
#define USR_STACK_SIZE_WORDS (RIOTEE_STACK_SIZE / sizeof(uint32_t))
#define SYS_STACK_SIZE (configMINIMAL_STACK_SIZE + 128)

#define UNUSED(X) ((void)(X))

enum {
  /* Capacitor voltage high threshold. */
  EVT_RUNTIME_PWRGD_L = EVT_RUNTIME_BASE + 0,
  /* Capacitor voltage low threshold. */
  EVT_RUNTIME_PWRGD_H = EVT_RUNTIME_BASE + 1,
};

extern unsigned long __etext;
extern unsigned long __bss_retained_start__;
extern unsigned long __bss_retained_end__;

extern unsigned long __data_retained_start__;
extern unsigned long __data_retained_end__;
extern unsigned long __data_start__;
extern unsigned long __data_end__;

/* Linker section where drivers register their teardown functions */
extern unsigned long __teardown_start__;
extern unsigned long __teardown_end__;

/* This marker is used to check if device has been reset before */
unsigned long fresh_marker = 0x8BADF00D;

StaticTask_t usr_task_tcb;
/* Put this into size-limited RETAINED_RAM region (see linker.ld) */
StackType_t usr_task_stack[USR_STACK_SIZE_WORDS] __attribute__((section(".usr_task_mem")));

StaticTask_t xIdleTaskTCB;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

StaticTask_t xSystemTaskTCB;
StackType_t uxSystemTaskStack[SYS_STACK_SIZE];

TaskHandle_t sys_task_handle;
TaskHandle_t usr_task_handle;

/* Runtime stats go into retained bss so they are automatically checkpointed. */
runtime_stats_t runtime_stats __attribute__((section(".retained_bss")));

void sys_setup_timer(unsigned int ticks);
void sys_cancel_timer(void);

/* Dummy callback to be called when low capacitor voltage is detected. Can be overwritten by the user. */
__attribute__((weak)) void suspend_callback(void){};
/* Dummy callback to be called when capacitor voltage has recovered. Can be overwritten by the user. */
__attribute__((weak)) void resume_callback(void){};
/* Dummy callback during first boot-up of the device */
__attribute__((weak)) void bootstrap_callback(void){};
/* Dummy callback after every reset */
__attribute__((weak)) void reset_callback(void){};

void __libc_init_array(void);

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
  while (NRF_NVMC->READY == 0)
    __NOP();
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
} checkpoint_header;

enum { NVM_SIG_VALID = 0xCAFED00D, NVM_SIG_INVALID = 0x8BADF00D };

/* Stores task stack and static/global variables in non-volatile memory. */
static int checkpoint_store() {
  int rc;
  checkpoint_header hdr;

  hdr.top_of_stack = *(uint32_t *)&usr_task_tcb;

  hdr.stack_size = ((unsigned int)&usr_task_stack[USR_STACK_SIZE_WORDS - 1] - hdr.top_of_stack) / sizeof(StackType_t);

  hdr.data_size = (unsigned int)&__data_retained_end__ - (unsigned int)&__data_retained_start__;

  hdr.bss_size = (unsigned int)&__bss_retained_end__ - (unsigned int)&__bss_retained_start__;

  /* We will write the snapshot with an invalid signature as the first value. This makes sure that no faulty snapshot is
   * loaded. */
  hdr.signature = NVM_SIG_INVALID;

  if ((rc = nvm_begin_write(0x0)) != 0)
    return rc;
  if ((rc = nvm_write((uint8_t *)&hdr, sizeof(checkpoint_header))) != 0)
    return rc;
  if ((rc = nvm_write((uint8_t *)hdr.top_of_stack, hdr.stack_size * sizeof(StackType_t))) != 0)
    return rc;
  if ((rc = nvm_write((uint8_t *)&__data_retained_start__, hdr.data_size)) != 0)
    return rc;
  if ((rc = nvm_write((uint8_t *)&__bss_retained_start__, hdr.bss_size)) != 0)
    return rc;

  nvm_end();

  /* Now that the snapshot was written successfully, we can update the signature */
  if ((rc = nvm_begin_write(0x0)) != 0)
    return rc;
  uint32_t signature = NVM_SIG_VALID;
  if ((rc = nvm_write((uint8_t *)&signature, sizeof(signature))) != 0)
    return rc;
  nvm_end();

  /* If this was a first boot, overwrite the marker now */
  if (check_fresh_start()) {
    overwrite_marker();
  }

  return 0;
}

/* Loads a snapshot from NVM into task stack and static/global variables. */
static int checkpoint_load() {
  int rc;
  checkpoint_header hdr;

  if ((rc = nvm_begin_read(0x0)) != 0)
    return rc;
  if ((rc = nvm_read((uint8_t *)&hdr, sizeof(checkpoint_header))) != 0)
    return rc;

  /* Check the signature to avoid loading garbage */
  if (hdr.signature != NVM_SIG_VALID) {
    nvm_end();
    return -1;
  }

  /* Restore stack */
  if ((rc = nvm_read((uint8_t *)hdr.top_of_stack, hdr.stack_size * sizeof(StackType_t))) != 0)
    return rc;
  /* Restore static/global variables */
  if ((rc = nvm_read((uint8_t *)&__data_retained_start__, hdr.data_size)) != 0)
    return rc;
  if ((rc = nvm_read((uint8_t *)&__bss_retained_start__, hdr.bss_size)) != 0)
    return rc;
  nvm_end();

  /* Copy top of stack into freertos TCB structure */
  memcpy(&usr_task_tcb, &hdr.top_of_stack, sizeof(uint32_t));
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

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  printf("\r\nPANIC: Stack has overflowed!\r\n");
  while (1) {
    enter_low_power();
  }
}
static void threshold_callback(unsigned int pin_no) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (pin_no == PIN_PWRGD_L) {
    xTaskNotifyIndexedFromISR(sys_task_handle, 1, EVT_RUNTIME_PWRGD_L, eSetValueWithOverwrite,
                              &xHigherPriorityTaskWoken);
  } else if (pin_no == PIN_PWRGD_H) {
    xTaskNotifyIndexedFromISR(sys_task_handle, 1, EVT_RUNTIME_PWRGD_H, eSetValueWithOverwrite,
                              &xHigherPriorityTaskWoken);
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  return;
}

/* Initializes 'retained' section when no checkpoint exists. */
static void initialize_retained(void) {
  volatile unsigned long *src, *dst;
  /* Get LMA of the data_retained section in flash. */
  src = &__etext + (&__data_end__ - &__data_start__);
  dst = &__data_retained_start__;
  while (dst < &__data_retained_end__)
    *(dst++) = *(src++);

  src = &__bss_retained_start__;
  while (src < &__bss_retained_end__)
    *(src++) = 0;

  /* Call static constructors via newlibc to re-setup the memory that just got cleared. */
  __libc_init_array();
}

/* Waits until capacitor is fully charged as indicated by PWRGD_H pin */
riotee_rc_t riotee_wait_cap_charged(void) {
#ifdef DISABLE_CAP_MONITOR
  return RIOTEE_SUCCESS;
#else
  return riotee_gpio_wait_level(PIN_PWRGD_H, RIOTEE_GPIO_LEVEL_HIGH, RIOTEE_GPIO_IN_NOPULL);
#endif
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
}

/* High priority system task initializes runtime, and handles intermittent execution and checkpointing. */
static void sys_task(void *pvParameter) {
  UNUSED(pvParameter);

  unsigned long notification_value;

  /* Make sure that the user task does not yet start */
  vTaskSuspend(usr_task_handle);

#ifndef DISABLE_CAP_MONITOR
  gpint_register(PIN_PWRGD_H, RIOTEE_GPIO_LEVEL_HIGH, RIOTEE_GPIO_IN_NOPULL, threshold_callback);
  xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
#endif

  if (check_fresh_start()) {
    initialize_retained();
    bootstrap_callback();

  } else {
    if (checkpoint_load() == 0) {
      /* Unblock the user task */
      xTaskNotifyIndexed(usr_task_handle, 1, EVT_RESET, eSetBits);
      runtime_stats.n_reset++;
    } else {
      initialize_retained();
      /* Call user bootstrap code */
      bootstrap_callback();
    }
  }

  reset_callback();

  for (;;) {
    resume_callback();
    vTaskResume(usr_task_handle);

#ifdef DISABLE_CAP_MONITOR
    vTaskSuspend(sys_task_handle);
#endif

    /* Wait until capacitor voltage falls below the 'low' threshold */
    gpint_register(PIN_PWRGD_L, RIOTEE_GPIO_LEVEL_LOW, RIOTEE_GPIO_IN_NOPULL, threshold_callback);
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);

    vTaskSuspend(usr_task_handle);
    teardown();
    /* Give the application an opportunity to switch off power-hungry devices */
    suspend_callback();

    runtime_stats.n_suspend++;

    /* Set a high threshold - upon reaching this threshold, execution continues */
    /* If the user task was already waiting on high threshold, we have to notify it here */
    if (gpint_unregister(PIN_PWRGD_H) == RIOTEE_GPIO_ERR_OK)
      xTaskNotifyIndexed(usr_task_handle, 1, EVT_GPIO_BASE, eSetBits);
    gpint_register(PIN_PWRGD_H, RIOTEE_GPIO_LEVEL_HIGH, RIOTEE_GPIO_IN_NOPULL, threshold_callback);

    /* Set a 100ms timer*/
    sys_setup_timer(3277);
    /* Wait until capacitor is recharged or timer expires */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
    /* Recharged? */
    if (notification_value == EVT_RUNTIME_PWRGD_H) {
      sys_cancel_timer();
      xTaskNotifyStateClearIndexed(xTaskGetCurrentTaskHandle(), 1);
      continue;
    }

    /* Timer has expired. Is capacitor voltage still below threshold? */
    if ((NRF_P0->IN & (1 << PIN_PWRGD_L)) == 0) {
      /* Take the snapshot */
      if (checkpoint_store() != 0)
        printf("Checkpointing failed!");

    } else {
      /* Monitor for capacitor voltage to drop below threshold again */
      gpint_register(PIN_PWRGD_L, RIOTEE_GPIO_LEVEL_LOW, RIOTEE_GPIO_IN_NOPULL, threshold_callback);
    }

    /* Wait until capacitor is recharged or discharged below the critical threshold again */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
    /* Recharged? */
    if (notification_value == EVT_RUNTIME_PWRGD_H) {
      gpint_unregister(PIN_PWRGD_L);
      continue;
    }
    /* Dropped below the threshold again -> take a snapshot */
    checkpoint_store();
    /* Wait until capacitor is recharged */
    xTaskNotifyWaitIndexed(1, 0xFFFFFFFF, 0xFFFFFFFF, &notification_value, portMAX_DELAY);
  }
}

int main(void);

void user_task(void *pvParameter) {
  UNUSED(pvParameter);
  main();
}

void runtime_start(void) {
  riotee_uart_init(PIN_D1, RIOTEE_UART_BAUDRATE_1000000);

  riotee_gpio_init();
  riotee_timing_init();

  nvm_init();

  usr_task_handle = xTaskCreateStatic(user_task, "USR", USR_STACK_SIZE_WORDS, NULL, tskIDLE_PRIORITY + 2,
                                      usr_task_stack, &usr_task_tcb);

  sys_task_handle = xTaskCreateStatic(sys_task, "SYS", SYS_STACK_SIZE, NULL, (configMAX_PRIORITIES - 1),
                                      uxSystemTaskStack, &xSystemTaskTCB);

  vTaskStartScheduler();
}
