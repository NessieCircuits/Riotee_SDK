#ifndef __GPINT_H_
#define __GPINT_H_

#include <stdint.h>
#include "nrf_gpio.h"

typedef enum { GPINT_LEVEL_LOW = 0, GPINT_LEVEL_HIGH = 1 } gpint_level_t;

typedef void (*GPINT_CALLBACK)(unsigned int);
typedef nrf_gpio_pin_pull_t gpint_pin_pull_t;

#ifdef __cplusplus
extern "C" {
#endif

int gpint_init(void);
int gpint_register(uint32_t pin, gpint_level_t level, gpint_pin_pull_t pull, GPINT_CALLBACK cb);
int gpint_unregister(uint32_t pin);
int gpint_wait(uint32_t pin, gpint_level_t level, gpint_pin_pull_t pull);

#ifdef __cplusplus
}
#endif

#endif /* __GPINT_H_ */