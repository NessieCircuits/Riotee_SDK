/**
 * @defgroup gpio GPIO driver
 */

#ifndef __RIOTEE_GPIO_H_
#define __RIOTEE_GPIO_H_

#include <stdint.h>
#include <stddef.h>

#define NRF_PORT0_ADDR 0x50000000UL
#define NRF_PORT1_ADDR 0x50000300UL

typedef struct {
  volatile uint32_t RESERVED[321];
  volatile uint32_t OUT;
  volatile uint32_t OUTSET;
  volatile uint32_t OUTCLR;
  volatile uint32_t IN;
  volatile uint32_t DIR;
  volatile uint32_t DIRSET;
  volatile uint32_t DIRCLR;
  volatile uint32_t LATCH;
  volatile uint32_t DETECTMODE;
  volatile uint32_t RESERVED1[118];
  volatile uint32_t PIN_CNF[32];
} riotee_gpio_port_t;

#define NRF_PORT0 ((riotee_gpio_port_t*)NRF_PORT0_ADDR)
#define NRF_PORT1 ((riotee_gpio_port_t*)NRF_PORT1_ADDR)

/**
 * @brief GPIO input pullup configuration.
 * \ingroup gpio
 */
typedef enum {
  /** Pullup resistor active. */
  RIOTEE_GPIO_IN_PULLUP = 3,
  /** Pulldown resisotr active. */
  RIOTEE_GPIO_IN_PULLDOWN = 1,
  /** No pullup/pulldown resistor active. */
  RIOTEE_GPIO_IN_NOPULL = 0,
} riotee_gpio_in_pull_t;

static inline riotee_gpio_port_t* riotee_gpio_get_port(unsigned int* pin) {
  if (*pin > 41)
    return NULL;
  if (*pin > 31) {
    *pin -= 32;
    return NRF_PORT1;
  }
  return NRF_PORT0;
}

/**
 * @brief Configures pin as output.
 *
 * @param pin Pin number.
 * \ingroup gpio
 */
static inline void riotee_gpio_cfg_output(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);

  /* Output, input buffer disconnected */
  reg->PIN_CNF[pin] = (1UL << 0) | (1UL << 1);
}

/**
 * @brief Configures pin as output.
 *
 * @param pin Pin number.
 * @param pull Type of pull resistor.
 * \ingroup gpio
 */
static inline void riotee_gpio_cfg_input(unsigned int pin, riotee_gpio_in_pull_t pull) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);

  /* Input buffer connected */
  reg->PIN_CNF[pin] = (pull << 2);
}

/**
 * @brief Sets output pin to logic high.
 *
 * @param pin Pin number.
 * \ingroup gpio
 */
static inline void riotee_gpio_set(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);
  reg->OUTSET = (1UL << pin);
}

/**
 * @brief Sets output pin to logic low.
 *
 * @param pin Pin number.
 * \ingroup gpio
 */
static inline void riotee_gpio_clear(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);

  reg->OUTCLR = (1UL << pin);
}

/**
 * @brief Toggles logic state of output pin.
 *
 * @param pin Pin number.
 * \ingroup gpio
 */
static inline void riotee_gpio_toggle(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);

  reg->OUT ^= (1UL << pin);
}

/**
 * @brief Reads logic state of input pin.
 *
 * @param pin Pin number.
 * @return uint32_t 1 if logic high, 0 if logic low
 * \ingroup gpio
 */
static inline uint32_t riotee_gpio_read(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);
  return (reg->IN >> pin) & 1UL;
}

/**
 * @brief Reads logic state of output pin.
 *
 * @param pin Pin number.
 * @return uint32_t 1 if logic high, 0 if logic low
 * \ingroup gpio
 */
static inline uint32_t riotee_gpio_is_set(unsigned int pin) {
  riotee_gpio_port_t* reg = riotee_gpio_get_port(&pin);
  return (reg->OUT >> pin) & 1UL;
}

#endif /** __RIOTEE_GPIO_H_ */
