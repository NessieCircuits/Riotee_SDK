#include "riotee.h"
#include "riotee_timing.h"

#include "external/ArduinoCore-API/api/Common.h"

void delay(long unsigned int ms) {
  riotee_sleep_ms(ms);
}

void delayMicroseconds(long unsigned us) {
  riotee_delay_us(us);
}