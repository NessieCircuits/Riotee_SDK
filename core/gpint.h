#ifndef __GPINT_H_
#define __GPINT_H_

#include <stdint.h>

typedef enum { GPINT_LEVEL_LOW = 0, GPINT_LEVEL_HIGH = 1 } gpint_level_t;
enum { GPINT_ERR_OK = 0, GPINT_ERR_GENERIC = 1, GPINT_ERR_BUSY = 2, GPINT_ERR_UNSUPPORTED = 3 };

typedef void (*GPINT_CALLBACK)(unsigned int);

typedef enum {
  RIOTEE_GPIO_PIN_NOPULL = GPIO_PIN_CNF_PULL_Disabled,    ///<  Pin pull-up resistor disabled.
  RIOTEE_GPIO_PIN_PULLDOWN = GPIO_PIN_CNF_PULL_Pulldown,  ///<  Pin pull-down resistor enabled.
  RIOTEE_GPIO_PIN_PULLUP = GPIO_PIN_CNF_PULL_Pullup,      ///<  Pin pull-up resistor enabled.
} riotee_gpio_pin_pull_t;

#ifdef __cplusplus
extern "C" {
#endif

int riotee_gpint_init(void);
int riotee_gpint_register(uint32_t pin, gpint_level_t level, riotee_gpio_pin_pull_t pull, GPINT_CALLBACK cb);
int riotee_gpint_unregister(uint32_t pin);
int riotee_gpint_wait(uint32_t pin, gpint_level_t level, riotee_gpio_pin_pull_t pull);

#ifdef __cplusplus
}
#endif

#endif /* __GPINT_H_ */