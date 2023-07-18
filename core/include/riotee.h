#ifndef __RIOTEE_H_
#define __RIOTEE_H_

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

/* This gets called right after startup */
void startup_callback(void);
/* This gets called one time after flashing new firmware */
void bootstrap_callback(void);
/* This gets called after every reset */
void reset_callback(void);
/* This gets called right before user code is suspended */
void turnoff_callback(void);

/* Waits until capacitor is fully charged as indicated by PWRGD_H pin */
int riotee_wait_cap_charged(void);

#define __VOLATILE_INITIALIZED __attribute__((section(".volatile.data")))
#define __VOLATILE_UNINITIALIZED __attribute__((section(".volatile.bss")))

#if defined __cplusplus
}
#endif

#endif /* __RIOTEE_H_ */
