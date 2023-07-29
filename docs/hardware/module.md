# Riotee Module

Our Riotee Module is the heart of the product line. It integrates energy harvesting, energy storage, power management, non-volatile memory, a powerful Cortex-M4 processor, and a 2.4-GHz, BLE-compatible radio into a tiny module with the footprint of a postage stamp. The Module can be soldered onto a PCB with any standard soldering iron or by reflow soldering. Thanks to the standard 0.1" pitch, it's also possible to mount some pin headers and plug the Module into a breadboard.

![Block diagram](./img/riotee-module-blockdiagram_full.svg)

A Maxim MAX20361 boost charger regulates the voltage of an attached harvester using Maximum Power Point tracking and charges the on-board capacitors. The boost converter can be configured over an I2C interface. The buck regulator outputs a constant 2V supply voltage. Two independent comparators continuously monitor the capacitor voltage against two software-controlled voltage thresholds.

The module has two microcontrollers: The Nordic Semiconductor nRF52833 has a 64-MHz Cortex-M4 CPU with floating point unit and a low-power 2.4GHz wireless radio. The TI MSP430FR5962 has 128kB non-volatile FRAM. Both controllers can be programmed by the user and share access to all other components of the system. For example, the buffered capacitor voltage is available on the ADC inputs of both controllers. The controllers are connected to each other with a 4-wire SPI bus plus an additional handshake line. This system architecture supports different configurations: The software provided by the Riotee SDK runs application and networking code on the nRF52833 microcontroller and uses the MSP430FR as an SPI-based non-volatile RAM to retain application state across power outages. Other users may prefer running application code directly on the MSP430FR controller and using the nRF52833 as an SPI-controlled radio.

## Features & Specifications

- 15.2 x 27.18 x 2.5mm, breadboard-friendly module with 11 GPIOs (2 with analog sensing)
- SPI, I²C, UART, PWM, and I²S can be flexibly mapped to any of the GPIOs
- 64-MHz ARM Cortex-M4F CPU with floating-point unit (Nordic nRF52833)
- 16MHz TI MSP430 microcontroller with 128kB non-volatile FRAM memory
- Low-power, 2.4-GHz radio
- Boost converter with software-defined [maximum power point tracking](https://en.wikipedia.org/wiki/Maximum_power_point_tracking) 
- Sensing of capacitor voltage, harvesting voltage, and harvesting current
- Software-defined thresholds to enable and disable capacitor-based power
- 66 uF expandable on-board capacitance
- 2 V regulated output voltage
- Additional capacitor backup powers RTC in the absence of a power supply (may last for seconds or minutes depending on conditions)
- Onboard LED

## Pinout

![Pinout](./img/riotee-module-pinout_comb.svg)

## Pin description

| Pad        | Description                                                                         |
|------------|-------------------------------------------------------------------------------------|
| D0         | Digital Input/Ouptut.                                                               |
| D1         | Digital Input/Ouptut.                                                               |
| D2/A0      | Digital Input/Ouptut or analog input.                                               |
| D3/A1      | Digital Input/Ouptut or analog input.                                               |
| D4         | Digital Input/Ouptut.                                                               |
| D5         | Digital Input/Ouptut.                                                               |
| D6         | Digital Input/Ouptut.                                                               |
| D7         | Digital Input/Ouptut                                                                |
| D8         | Digital Input/Ouptut.                                                               |
| D9         | Digital Input/Ouptut.                                                               |
| D10        | Digital Input/Ouptut.                                                               |
| SCL        | I2C Clock. Connected to AM1805 RTC and MAX20361 boost. Connect I2C devices here.    |
| SDA        | I2C Data. Connected to AM1805 RTC and MAX20361 boost. Connect I2C devices here.     |
| VcapMon    | Buffered capacitor voltage. Use this to measure capacitor voltage with peripherals. |
| Vin        | Harvesting input. Connect a DC voltage between 0.25V and 2.5V.                      |
| Vcap       | Capacitor voltage. Connect additional capacitance to this pad.                      |
| +2V        | Main power supply. Connect peripherals to this pad.                                 |
| +2V (Aon)  | Output of the Buck converter. Not disabled by power switches.                       |
| Vshunt     | Supply voltage behind power switch. Measure current between +2V (Aon) and this pad. |
| Bypass     | Bypass enable. Set pad high to enable power switch for current measurement.         |
| EnableBuck | Enable/Disable buck regulator. Use this when not using MAX20361 boost regulator.    |
| SwdClk     | ARM Serial Wire Debug (SWD) Clock for programming nRF52.                            |
| SwdIO      | ARM Serial Wire Debug (SWD) I/O for programming nRF52.                              |
| SbwClk     | TI Spy-bi-wire (SBW) Clock for programming MSP430FR.                                |
| SbwIO      | TI Spy-bi-wire (SBW) I/O for programming MSP430FR.                                  |

The nRF52 allows flexbily mapping most of the peripherals to any of the pins.

## Soldering

The Riotee Module can be soldered onto a PCB manually or in an automated reflow process.
For reflow soldering, we recommend a standard lead free process with a peak temperature of 260° Celsius.
The pads on the bottom of the module can only be connected via reflow soldering and require special attention:
If applying paste with a stencil, use a stencil with a thickness of at least 100um, preferably more.
When using a solder paste printer, you may need to increase the default amount of paste for these pads by up to 50%.

## Programming

For programming the nRF52 and MSP430FR on-board the module, use the {doc}`/hardware/probe`.
Connect ground and *VccTarget* on the probe to *+2V* on the Riotee Module.
For programming the nRF52, connect *SwdClk* and *SwdIO* between the probe and the module.
For programming the MSP430, connect *SbwClk* and *SbwIO* between the probe and the module.

Install the `riotee-probe` Python package with

```bash
pipx install riotee-probe
```

Flash new firmware with

```bash
riotee-probe program -d nrf52 -f firmware_nrf52.hex
```

and 

```bash
riotee-probe program -d msp430 -f firmware_msp430.hex
```

## LED

The Riotee Module has a red LED connected to *P0.03* of the nRF52 and *PJ.0* of the MSP430FR.
Set the pin high to switch on the LED.

## Capacitor Voltage Monitoring

The capacitor voltage is divided down with a voltage divider and made available to the system via an op-amp buffer as *VcapMon*.

```{math}
VcapMon=\frac{5.62}{15.62}Vcap
```

*VcapMon* is connected to ADC channel AIN5 on the nRF52 and ADC channel A17 on the MSP430.

## Power Switches

```{image} img/power_switches.svg
:width: 50%
:align: center
```

There are two power switches between the output of the buck regulator and the other parts of the system.
One switch is controlled by the *Bypass* signal and allows rerouting current through an external amperemeter to measure the dynamic power consumption of the module.
When setting high pin *Bypass*, the switch opens and current can be measured with an amperemeter connected between *+2V (Aon)* and *Vshunt*.
See also [here](measuring_current).

The other switch is controlled by the AM1805 RTC.
It allows entering an ultra-low power mode where all components are switched off except for the RTC.
This reduces current consumption on the +2V supply to <45nA.
For details, refer to the [AM1805 datasheet](https://www.solid-run.com/wiki/lib/exe/fetch.php?media=imx8:carrierboard:docs:sr-imx8m-carrierboard-onboard-rtc.pdf).


## Resources
 - [3D step model](https://github.com/NessieCircuits/Riotee_Module/blob/main/RioteeModule.step)
 - [KiCad schematic symbol](https://github.com/NessieCircuits/Riotee_Module/blob/main/RioteeModule.kicad_sym)
 - [KiCad footprint](https://github.com/NessieCircuits/Riotee_Module/blob/main/RioteeModule.pretty)