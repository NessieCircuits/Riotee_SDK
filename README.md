[![Build](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/make.yml/badge.svg)](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/make.yml)

# A battery-free runtime for the Riotee platform

The code in this repository allows executing user-code on the battery-free Riotee module.
For a detailed description refer to the [documentation](https://www.riotee.nessie-circuits.de/docs/software/riotee-runtime).

## Features

 - Capacitor voltage monitoring
 - Driver for non-volatile RAM
 - Automatic checkpointing of user application
 - C++ support
 - Basic timing support
 - printf support
 - BLE advertising
 - I2C driver
 - UART driver
 - Driver for MAX20361 boost converter
 - ADC driver
 - Stella wireless protocol for bidirectional communication with a basestation

## Usage

There are three ways how you can use this runtime. 

- Use our Arduino package to conveniently program your Riotee hardware. The Arduino IDE will automatically include the runtime and you develop your application as a sketch.
- Write your own application code and link it against the runtime as a static library.
- Copy your code into the codebase of the runtime and build everything together.

## Building

Clone this repository and its submodules:

```
git clone git@github.com:NessieCircuits/Riotee_Runtime.git
git submodule init
git submodule update
```

Install the `gnu-arm-none-eabi` toolchain and `GNU make`. If the toolchain is not on your path, set the `GNU_INSTALL_ROOT` environment variable accordingly.

To build the library
```
make lib
```

To build the demo application

```
make app
```

To upload the resulting firmware to a Riotee board or Riotee module (via a Riotee Probe), you need to have `pyOCD` installed. Upload the firmware with:

```
make flash
```

## Code structure

 - `startup.c`: Startup code
 - `runtime.c`: FreeRTOS based intermittent runtime
 - `nvm.c`: Driver for MSP430FR non-volatile RAM
 - `timing.c`: Basic delay functions via on-board RTC
 - `radio.c`: Basic radio driver; can be used with different protocols
 - `ble.c`: Implementation of BLE undirected non-connectable advertising
 - `i2c.c`: I2C driver
 - `max20361.c`: Driver for MAX20361 boost-converter; uses I2C driver
 - `uart.c`: UART driver
 - `gpint.c`: Driver for low power GPIO interrupts
 - `printf.c`: Marco Paland's tiny printf
