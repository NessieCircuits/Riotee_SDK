#ifndef __GPINT_H_
#define __GPINT_H_

#include <stdint.h>

typedef enum { GPINT_LEVEL_LOW = 0, GPINT_LEVEL_HIGH = 1 } gpint_level_t;

typedef void (*GPINT_CALLBACK)(unsigned int);

#ifdef __cplusplus
extern "C" {
#endif

int gpint_init(void);
int gpint_register(uint32_t pin, gpint_level_t level, GPINT_CALLBACK cb);
int gpint_unregister(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* __GPINT_H_ */