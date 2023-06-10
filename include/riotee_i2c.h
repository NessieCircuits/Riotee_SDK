#ifndef __I2C_H_
#define __I2C_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int i2c_init(unsigned int pinSDA, unsigned int pinSCL);
int i2c_write(uint8_t dev_addr, uint8_t *data, size_t n_data);
int i2c_read(uint8_t *buffer, size_t n_data, uint8_t dev_addr);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H_ */