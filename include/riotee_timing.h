#ifndef __TIMING_H_
#define __TIMING_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int timing_init(void);

/* Sleep for number of 32768kHz ticks */
void sleep_ticks(unsigned int ticks);
void sleep_ms(unsigned int ms);

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

void sys_setup_timer(unsigned int ticks);
void sys_cancel_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMING_H_ */