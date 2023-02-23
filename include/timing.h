#ifndef __TIMING_H_
#define __TIMING_H_
#include "nrfx.h"

#include <soc/nrfx_coredep.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define nrf_delay_us(us_time) nrfx_coredep_delay_us(us_time)

static inline void nrf_delay_ms(uint32_t ms_time) {
  if (ms_time == 0) {
    return;
  }

  do {
    nrf_delay_us(1000);
  } while (--ms_time);
}

void delay(unsigned int ms);
int timing_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMING_H_ */