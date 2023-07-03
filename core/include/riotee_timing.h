#ifndef __TIMING_H_
#define __TIMING_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the LF clock. Must be called once after reset before any sleep/delay calls.
 *
 */
void riotee_timing_init(void);

/**
 * @brief Waits in a low power mode for the specificed number of ticks on a 32kHz clock.
 *
 * Starts a timer and blocks on a notification. The idle task will keep the system in a low power mode. Function may
 * return earlier than requested if the power runs out and a reset occurs.
 *
 * @param ticks_32k Number of ticks on a 32kHz clock to sleep for.
 *
 * @return 0 if successful, <0 if a reset occurs while waiting for the notification.
 */
int riotee_sleep_ticks(unsigned int ticks_32k);

/**
 * @brief Waits in a low power mode for the specificed time.
 *
 * Starts a timer and blocks on a notification. The idle task will keep the system in a low power mode. Function may
 * return earlier than requested if the power runs out and a reset occurs.
 *
 * @param ms Number of milliseconds to sleep for.
 *
 * @return 0 if successful, <0 if a reset occurs while waiting for the notification.
 */
int riotee_sleep_ms(unsigned int ms);

/**
 * @brief Busy waits for specified number of microseconds.
 *
 * Keeps the CPU busy for the specified time. Consumes significant energy while waiting.
 *
 * @param us Number of microseconds to wait.
 */
void riotee_delay_us(unsigned int us);

/**
 * @brief Busy waits for specified number of milliseconds.
 *
 * Keeps the CPU busy for the specified time. Consumes significant energy while waiting.
 *
 * @param ms Number of milliseconds to wait.
 */
void riotee_delay_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* __TIMING_H_ */