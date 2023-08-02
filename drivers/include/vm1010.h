#ifndef __VM1010_H_
#define __VM1010_H_

#include <stdint.h>
#include "riotee.h"

typedef struct {
  unsigned int pin_mode;
  unsigned int pin_dout;
  unsigned int pin_vout;
  unsigned int pin_vbias;
} vm1010_cfg_t;

int vm1010_init(vm1010_cfg_t *cfg);
void vm1010_exit(void);
riotee_rc_t vm1010_sample(int16_t *result, unsigned int n_samples, unsigned int sample_interval_ticks32);
riotee_rc_t vm1010_wait4sound(void);

#endif /* __VM1010_H_ */