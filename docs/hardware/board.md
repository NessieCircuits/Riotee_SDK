# Riotee Board


Our Riotee Board combines a Riotee Module with a USB Type-C connector and circuitry that facilitates programming and debugging. Two 0.1-inch pin sockets expose all signals from the Riotee module, including 11 GPIOs that support I²C, SPI, and analog sensor applications. You can also use these headers to connect multiple Riotee Shields if you want to extend the capabilities of your device without designing a custom PCB. The Riotee Board has a push button, an LED, and a connector for a solar panel. Once your firmware has been flashed to the board, it will automatically switch over to untethered, battery-free operation.

## Features & Specifications

- 56 x 23 mm board with two 0.1-inch expansion headers for shields
- USB Type-C connector for communicating with a computer
- Raspberry Pi RP2040 controller to handle programming and debugging 
- CMSIS-DAP compatible for programming the Riotee Module via pyOCD or OpenOCD
- Makes UART output from Riotee Module available via USB
- Push button and additional user LED

## Pinout

![Board Pinout](./img/riotee-board-pinout.svg)


## Button

The Board has a button and an LED that are connected to the Riotee Module's pins.

The Button is connected to pin D6 and is pulled high with a 1M resistor.
This leads to an additional current consumption of around 2uA when pin D6 is driven low (default configuration).

## LED

The LED is powered from the USB supply and is controlled with pin D5 via an n-channel mosfet.
The LED only works when the Riotee Board is connected to USB and does not consume energy from the capacitor.
D5 is pulled low with a 1M resistor, leading to an additional current consumption of around 2uA when pin D5 is driven high.

## UART output

The UART output from pin D1/TX of the Riotee Module is being made available via USB by the RP2040 on the Riotee board.
When connecting the Riotee Board to your computer, a serial port should appear.
Use your favorite terminal application (e.g. minicom on Linux or putty on Windows) to connect to the serial port.
The default baudrate of the UART driver is 1000000bps, but this can be changed via the API.

## Resources
 - [Schematics](https://www.riotee.nessie-circuits.de/artifacts/board/latest/schematics.pdf)
 - [Layout](https://www.riotee.nessie-circuits.de/artifacts/board/latest/pcb.pdf)
 - [Assembly plan](https://www.riotee.nessie-circuits.de/artifacts/board/latest/assembly.pdf)
 - [3D rendering](https://www.riotee.nessie-circuits.de/artifacts/board/latest/3drendering.png)