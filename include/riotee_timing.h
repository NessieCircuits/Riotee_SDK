#ifndef __TIMING_H_
#define __TIMING_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int riotee_timing_init(void);

/* Sleep for number of 32768kHz ticks */
void riotee_sleep_ticks(unsigned int ticks_32k);
void riotee_sleep_ms(unsigned int ms);

void riotee_delay_us(unsigned int us);
void riotee_delay_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* __TIMING_H_ */