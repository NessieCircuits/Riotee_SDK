#ifndef __WIRE_H_
#define __WIRE_H_

#include "external/ArduinoCore-API/api/HardwareI2C.h"
#include "external/ArduinoCore-API/api/RingBuffer.h"

#define I2C_BUFFER_SIZE 32

class RioteeI2C : public arduino::HardwareI2C {
 public:
  RioteeI2C(void){};
  void begin();
  void begin(uint8_t address);

  void end(){};

  void setClock(uint32_t freq);

  void beginTransmission(uint8_t address);
  uint8_t endTransmission(void);
  uint8_t endTransmission(bool stopBit);

  size_t requestFrom(uint8_t address, size_t len);
  size_t requestFrom(uint8_t address, size_t len, bool stopBit);

  int read(void);
  size_t write(uint8_t data);
  size_t write(uint8_t* data, size_t len);

  void onReceive(void (*)(int)){};
  void onRequest(void (*)(void)){};

  int available(void);
  int peek(void);

 private:
  arduino::RingBufferN<I2C_BUFFER_SIZE> tx_ringbuf;
  arduino::RingBufferN<I2C_BUFFER_SIZE> rx_ringbuf;

  uint8_t tx_address;
};

extern RioteeI2C Wire;

#endif /* __WIRE_H_ */