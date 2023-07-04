#ifndef __SERIAL_H_
#define __SERIAL_H_

#include "external/ArduinoCore-API/api/Stream.h"

class SerialClass : public Print {
public:
  void begin(unsigned long);
  int peek();
  int available();
  int read();
  void flush();
  size_t write(const uint8_t *, size_t);
  size_t write(uint8_t);
};

extern SerialClass Serial;

#endif /* __SERIAL_H_ */