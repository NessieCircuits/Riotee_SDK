#ifndef Arduino_h
#define Arduino_h

#include "riotee.h"
#include "riotee_timing.h"
#include "riotee_spic.h"
#include "riotee_i2c.h"
#include "riotee_adc.h"
#include "riotee_uart.h"
#include "printf.h"

#include "external/ArduinoCore-API/api/Common.h"

#ifdef __cplusplus
#include "Serial.h"
#endif

#define LED_BUILTIN PIN_LED_CTRL

#define D10 PIN_D10
#define D9 PIN_D9
#define D8 PIN_D8
#define D7 PIN_D7
#define D6 PIN_D6
#define D5 PIN_D5
#define D4 PIN_D4
#define D3 PIN_D3
#define D2 PIN_D2

#define D0 PIN_D0

#define A0 PIN_D2
#define A1 PIN_D3

#endif  // Arduino_h
