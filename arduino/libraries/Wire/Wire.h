#ifndef __WIRE_H_
#define __WIRE_H_

#include "HardwareI2C.h"

#define I2C_BUFFER_SIZE 64

class RioteeI2C : public arduino::HardwareI2C {
 public:
  void begin();

  void setClock(uint32_t freq);

  void beginTransmission(uint8_t address);
  uint8_t endTransmission(void);

  size_t requestFrom(uint8_t address, size_t len);

  int read(void);
  size_t write(uint8_t data);
  size_t write(uint8_t* data, size_t len);

 private:
  uint8_t tx_buffer[I2C_BUFFER_SIZE];
  uint8_t rx_buffer[I2C_BUFFER_SIZE];

  unsigned int rx_buffer_idx;
  unsigned int tx_buffer_idx;

  uint8_t tx_address;
};

#endif /* __WIRE_H_ */