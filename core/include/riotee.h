#ifndef __BOARD_H_
#define __BOARD_H_

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
int wait_until_charged(void);

#define __VOLATILE_INITIALIZED __attribute__((section(".volatile.data")))
#define __VOLATILE_UNINITIALIZED __attribute__((section(".volatile.bss")))

#if defined __cplusplus
}
#endif

#endif /* __BOARD_H_ */
