#ifndef Arduino_h
#define Arduino_h

#include "riotee.h"
#include "riotee_timing.h"
#include "printf.h"

#ifdef __cplusplus
#include "Serial.h"
#endif

#define LOW (0x0)
#define HIGH (0x1)

#define INPUT (0x0)
#define OUTPUT (0x1)
#define INPUT_PULLUP (0x2)
#define INPUT_PULLDOWN (0x3)

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

#ifdef __cplusplus
extern "C" {
#endif

void pinMode(uint32_t pin, uint32_t mode);
void digitalWrite(uint32_t ulPin, uint32_t ulVal);

#define delay(x) riotee_sleep_ms(x)
#define delayMicroseconds(x) riotee_delay_us(x)

int analogRead(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif  // Arduino_h
