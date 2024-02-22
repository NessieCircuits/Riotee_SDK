# Riotee documentation

Every year millions of new portable IoT devices are sold, and they are all powered by batteries.
Regularly replacing millions of batteries is inconvenient, expensive, and bad for the environment.

Riotee is an open-source hardware and software platform for building IoT devices that rely entirely on energy that can be harvested from sources like small solar panels. No batteries included.

## Hardware ecosystem

Riotee products are available for purchase on [CrowdSupply](https://www.crowdsupply.com/nessie-circuits/riotee).

 - The [Riotee Module](./hardware/module.md) integrates energy harvesting, capacitor storage, power management, non-volatile memory, a powerful Cortex-M4 processor, and a 2.4-GHz, BLE-compatible radio into a tiny module with the footprint of a postage stamp.
 - The [Riotee Board](./hardware/board.md) combines a Riotee Module with a USB Type-C connector and circuitry that facilitates programming and debugging. Two 0.1-inch pin sockets expose all signals from the Riotee module, including 11 GPIOs that support I²C, SPI, and analog sensor applications.
 - The [Solar Shield](./hardware/solar_shield.md) plugs into the Riotee Board's headers. It has four small solar cells and a pair of sliding switches that allow you to select one, three, or all four solar cells so you can experiment to see which configuration works best under specific conditions.
 - The [Sensor Shield](./hardware/sensor_shield.md) adds an accelerometer, a temperature-and-humidity sensor, and a microphone. 
 - The [Capacitor Shield](./hardware/capacitor_shield.md) allows you to extend the on-board capacitance of the Riotee device.
 - The [Riotee Probe](./hardware/probe.md) enables programming the microcontrollers on the Riotee Module after it has been soldered to a PCB.

## Software support

 - For a quickstart with Riotee, use the [Arduino package](arduino).
 - For more involved development, use the [SDK](https://github.com/NessieCircuits/Riotee_SDK) and the [template project](https://github.com/NessieCircuits/Riotee_AppTemplate).


```{eval-rst}
.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basics
   quickstart
   hardware/index
   software/index
   networking/index
   debugging
   current_consumption
   support
   legal_notice
```
