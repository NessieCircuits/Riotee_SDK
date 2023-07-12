# Riotee Probe

Programming battery-free devices is tricky. The power from a harvester is generally insufficient to support programming. Furthermore, the additional current consumption and the leakage through the programming pins can interfere with the behavior of a battery-free device. But connecting the device to a constant power supply prevents observing its *real* behavior. For many years, our solution to this problem was to attach the device to a power supply and programmer, flash the software, remove the power supply and the programmer, test the new firmware, connect the wires again and so on. This is inconvenient and frustrating when you're chasing a bug.

With the Riotee Probe the process becomes much easier: When programming/debugging is requested, the Probe automatically switches on a constant power supply and supplies the microcontrollers on the Riotee Module throughout the transfer. After programming, the power supply as well as the programming pins are automatically disconnected from the Riotee Module with analog switches. The device returns to harvesting operation and you can immediately observe the new software without interference from the programming dongle.

The Riotee probe supports programming both, the MSP430 and the nRF52 on the Riotee Module after you've soldered it onto your PCB or plugged it into a breadboard. It has a standard, 10-pin, 0.1-inch connector and is compatible with Tag-Connect cables that support in-circuit debugging.

## Features & Specifications

- Custom debug probe for in-circuit programming and debugging of Riotee Modules
- USB Type-C connector for communicating with a computer
- Raspberry Pi RP2040 controller handles programming and debugging
- CMSIS-DAP compatible for programming the Riotee Module via pyOCD or OpenOCD
- Makes UART output from Riotee module available via USB


## Resources
 - [Schematics](https://www.riotee.nessie-circuits.de/artifacts/probe_hardware/latest/schematics.pdf)
 - [Layout](https://www.riotee.nessie-circuits.de/artifacts/probe_hardware/latest/pcb.pdf)
 - [Assembly plan](https://www.riotee.nessie-circuits.de/artifacts/probe_hardware/latest/assembly.pdf)
 - [3D rendering](https://www.riotee.nessie-circuits.de/artifacts/probe_hardware/latest/3drendering.png)
