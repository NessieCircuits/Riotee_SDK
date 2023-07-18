# Riotee Sensor Shield

Our Sensor Shield adds an accelerometer, a temperature-and-humidity sensor, and a microphone to your Riotee device.

Sensors:
- Bosch BMA400 digital accelerometer
- Sensirion SHTC3 digital temperature and humidity sensor
- Vesper VM1010 analog microphone with power switch

## Low power operation

To minimize the power consumption of the shield, set pin D5 high and put the SHTC3 into sleep mode with `shtc3_init()` in the `startup_callback()` and whenever not needed.

For an example, take a look at the [VM1010 example](https://github.com/NessieCircuits/Riotee_Runtime/examples/vm1010).

## Pinout

Pin,Name,Type,Description
D5,MIC_DISABLE,Disables the microphone's power supply
D10,MIC_MODE,Enables VM1010 microphone's wake-on-sound mode
D4,MIC_DOUT,VM1010 microphone's sound detected output

## Resources
 - [Schematics](https://www.riotee.nessie-circuits.de/artifacts/sensor_shield/latest/schematics.pdf)
 - [Layout](https://www.riotee.nessie-circuits.de/artifacts/sensor_shield/latest/pcb.pdf)
 - [3D rendering](https://www.riotee.nessie-circuits.de/artifacts/sensor_shield/latest/3drendering.png)