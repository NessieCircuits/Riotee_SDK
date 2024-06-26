#ifndef __RIOTEE_H_
#define __RIOTEE_H_

#include "nrf.h"

#define PIN_SYS_SCL 40
#define PIN_SYS_SDA 6

#define PIN_D0 21
#define PIN_D1 8
#define PIN_D2 4
#define PIN_D3 5
#define PIN_D4 41
#define PIN_D5 26
#define PIN_D6 35
#define PIN_D7 11
#define PIN_D8 13
#define PIN_D9 16
#define PIN_D10 12

#define PIN_THRCTL_H1 34
#define PIN_THRCTL_L1 36
#define PIN_THRCTL_H0 9
#define PIN_THRCTL_L0 39

#define PIN_PWRGD_L 23
#define PIN_PWRGD_H 7

#define PIN_VCAP_SENSE 29

/* Buffered VCAP is on AIN5 */
#define AIN_VCAP_SENSE 5
/* A0 is on AIN2 */
#define AIN_A0 2
/* A1 is on AIN3 */
#define AIN_A1 3

#define PIN_RTC_INT 30
#define PIN_MAX_INT 25

#define PIN_LED_CTRL 3

#define PIN_C2C_MISO 14
#define PIN_C2C_MOSI 17
#define PIN_C2C_CLK 18
#define PIN_C2C_CS 22
#define PIN_C2C_GPIO 15

#define RIOTEE_RC_BASE 0x00000000
#define RIOTEE_RC_STELLA_BASE 0x01000000
#define RIOTEE_RC_I2C_BASE 0x02000000

/**
 * @defgroup riotee Riotee basics
 *  @{
 */

/** API return codes. */
enum {
  /** Operation completed successfully. */
  RIOTEE_SUCCESS = -(RIOTEE_RC_BASE + 0),
  /** Operation failed with generic error. */
  RIOTEE_ERR_GENERIC = -(RIOTEE_RC_BASE + 1),
  /** One or more invalid function arguments. */
  RIOTEE_ERR_INVALIDARG = -(RIOTEE_RC_BASE + 2),
  /** Operation was interrupted due to critically low capacitor voltage. */
  RIOTEE_ERR_TEARDOWN = -(RIOTEE_RC_BASE + 3),
  /** A reset occured during the operation. Most likely due to low capacitor voltage. */
  RIOTEE_ERR_RESET = -(RIOTEE_RC_BASE + 4),
  /** The requested operation would overflow a buffer. */
  RIOTEE_ERR_OVERFLOW = -(RIOTEE_RC_BASE + 5)
};

/** Common return code for Riotee SDK API functions. */
typedef int riotee_rc_t;

#if defined __cplusplus
extern "C" {
#endif

/**
 * @brief User provided startup callback.
 *
 * \ingroup riotee
 *
 * This callback is called immediately after every reset. It is called early during the startup process before the
 * system is fully initialized. Use this callback to put any power-hungry peripherals into a low-power mode. You must
 * not run any code that uses static/global variables, as they are not yet initialized. Keep this callback as short as
 * possible.
 *
 */
void earlyinit(void);

/**
 * @brief User-provided bootstrap callback.
 *
 * \ingroup riotee
 *
 * This callback is called once at the first reset after programming the device. It is called right before the first
 * lateinit. Use this callback to do any one-time initilization like setting an RTC or writing configuration to
 * the NVM.
 *
 */
void bootstrap(void);

/**
 * @brief User-provided reset callback.
 *
 * \ingroup riotee
 *
 * This callback is called after every reset of the device. Use this callback to initializer peripherals and drivers.
 *
 */
void lateinit(void);

/**
 * @brief User-provided suspend callback.
 *
 * \ingroup riotee
 *
 * This callback is called when the capacitor voltage drops below a threshold. If your application uses power-hungry
 * peripherals, power them off immediately in this callback. Keep the callback as short as possible.
 */
void suspend(void);

/**
 * @brief User-provided resume callback.
 *
 * \ingroup riotee
 *
 * This callback is called when the capacitor voltage has recovered and the application resumes.
 */
void resume(void);

/**
 * @brief Waits until the capacitor is charged.
 *
 * \ingroup riotee
 *
 * Puts the device into a low-power mode until the capacitor voltage reaches a software-defined 'high' threshold.
 * Returns immediately if the capacitor voltage already exceeds the threshold.
 *
 * @retval RIOTEE_SUCCESS       Capacitor voltage is above the 'high' threshold.
 * @retval RIOTEE_ERR_RESET     Reset occured while waiting.
 */
riotee_rc_t riotee_wait_cap_charged(void);

/**
 * @brief Stores a snapshot of the application state to non-volatile memory
 *
 * \ingroup riotee
 *
 * Requests the runtime to checkpoint application state to non-volatile memory. User should only call this when
 * capacitor is sufficiently charged.
 *
 */
void riotee_checkpoint();

/**
 * @brief Enter low power mode
 *
 * Puts the CPU to low power mode, Execution stops until CPU is woken up by an event.
 *
 */
__attribute__((always_inline)) static inline void enter_low_power(void) {
  __WFE();
  __SEV();
  __WFE();
}

/** Data is set to initial value after every reset. */
#define __NONRETAINED_INITIALIZED__ __attribute__((section(".volatile_data")))
/** Data is set to zero after every reset. */
#define __NONRETAINED_ZEROED__ __attribute__((section(".volatile_bss")))

#if defined __cplusplus
}
#endif

#endif /** @} __RIOTEE_H_ */
