#ifndef __SPI_H_
#define __SPI_H_

#include "external/ArduinoCore-API/api/HardwareSPI.h"

class RioteeSPI : public arduino::HardwareSPI {
 public:
  void begin(){};
  void end(){};

  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);

  void beginTransaction(void);
  void beginTransaction(arduino::SPISettings settings);
  void beginTransaction(arduino::SPISettings settings, unsigned int pin_sck, unsigned int pin_copi,
                        unsigned int pin_cipo);
  void beginTransaction(arduino::SPISettings settings, unsigned int pin_cs, unsigned int pin_sck, unsigned int pin_copi,
                        unsigned int pin_cipo);
  void endTransaction(void){};

  void usingInterrupt(int interruptNumber){};
  void notUsingInterrupt(int interruptNumber){};
  void attachInterrupt(){};
  void detachInterrupt(){};
};

extern RioteeSPI SPI;

#endif /* __SPI_H_ */