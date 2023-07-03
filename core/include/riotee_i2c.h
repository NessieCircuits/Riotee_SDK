#ifndef __RIOTEE_I2C_H_
#define __RIOTEE_I2C_H_

#include <stddef.h>
#include <stdint.h>

typedef enum {
  RIOTEE_I2C_FREQ_100K = 0x01980000UL,
  RIOTEE_I2C_FREQ_250K = 0x04000000UL,
  RIOTEE_I2C_FREQ_400K = 0x06400000UL,

} riotee_i2c_freq_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes I2C peripheral. Must be called once after reset before I2C is used.
 *
 * @param pin_sda SDA pin.
 * @param pin_scl SCL pin.
 */
void riotee_i2c_init(unsigned int pin_sda, unsigned int pin_scl);

/**
 * @brief Transmits the given data to an I2C peripheral.
 *
 * @param dev_addr I2C address of peripheral device.
 * @param data Pointer to data.
 * @param n_data Size of data buffer.
 * @return int 0 on success, <0 otherwise
 */
int riotee_i2c_write(uint8_t dev_addr, uint8_t *data, size_t n_data);

/**
 * @brief Reads the specified number of bytes from an I2C peripheral.
 *
 * @param buffer Pointer to a destination buffer where data gets stored.
 * @param n_data Number of bytes to read.
 * @param dev_addr I2C address of peripheral device.
 * @return int 0 on success, <0 otherwise
 */
int riotee_i2c_read(uint8_t *buffer, size_t n_data, uint8_t dev_addr);

/**
 * @brief Sets the I2C clock frequency.
 *
 * @param freq
 */
void riotee_i2c_set_freq(riotee_i2c_freq_t freq);

#ifdef __cplusplus
}
#endif

#endif /* __RIOTEE_I2C_H_ */