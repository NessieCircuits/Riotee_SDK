#ifndef __SHTC3_H_
#define __SHTC3_H_

#include <stdint.h>

typedef struct {
  float temp;
  float humidity;
} shtc3_res_t;

int shtc3_init(void);
int shtc3_id(uint16_t *id);
int shtc3_read(shtc3_res_t *res);

#endif /* __SHTC3_H_ */
