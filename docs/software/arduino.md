(arduino)=
# Arduino

The quickest way to use Riotee is with the Riotee Arduino package.
For installation and usage instructions, refer to the [quickstart guide](arduino_install).

The package supports most of the standard Arduino API like *digitalWrite(..)*, *analogRead(..)*, *delay(..)* and *Serial.print(..)*.
It also partially supports the *SPI* and *Wire* libraries.
Additionally, all drivers and the runtime API of the SDK are available.

The Arduino *setup(..)* function is called from the runtime's *lateinit(..)*.
The Arduino *loop(..)* function is continously run within the user task's *main(..)* function.
The runtime's additional callback functions are also available.
For more details, refer to the [runtime documentation](runtime_callbacks).

## Retained Memory and Stack Size

Depending on your application, you may need to increase the size of the stack size or the area of memory that is checkpointed before an impeding power failure ([read more](retained_memory)).
After installing the Arduino package and selecting *Riotee Board* as your board, you should find two menu entries *Retained Memory Size* and *Stack Size* where you can change these parameters.

## Delay

*delay(..)* is implemented as low power sleep operation. It maps to the *riotee_sleep_ms(..)* function, described [here](timing).

## ADC

Simple analog sampling is supported via Arduino's `analogRead(..)` function.
For more involved use cases, use the [ADC driver API](adc_driver).

## SPI

SPI controller functionality is supported via the [Arduino SPI API](https://www.arduino.cc/reference/en/language/functions/communication/spi/).

## I2C

I2C controller functionality is supported via the [Arduino Wire API](https://www.arduino.cc/reference/en/language/functions/communication/wire/)

## Serial

Uni-directional serial communication with configurable baudrate is supported via *Serial.begin(baudrate)* and *Serial.print(..)*.

## BLE

BLE advertising is supported via a simplified C++ interface:

```C
#include "RioteeBLE.h"

unsigned int counter = 0;

void setup(){
    BLE.begin("RIOTEE", &counter, sizeof(counter));
}

void loop(){
    BLE.advertise();
    delay(1000);
}
```
For more involved use cases, use the [SDK's BLE driver](ble_driver).