#ifndef __I2C_H_
#define __I2C_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int riotee_i2c_init(unsigned int pin_sda, unsigned int pin_scl);
int riotee_i2c_write(uint8_t dev_addr, uint8_t *data, size_t n_data);
int riotee_i2c_read(uint8_t *buffer, size_t n_data, uint8_t dev_addr);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H_ */