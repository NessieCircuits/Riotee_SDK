# Development setup

Although the Riotee SDK takes much of the pain of developing battery-free applications away, it remains much more challenging than developing for battery-powered devices.
An effective development setup is therefore essential.

## Power supply

While developing, a reliable, constant energy input helps to keep results comparable.
Nothing is more frustrating than hunting a phantom bug for hours when your application suddenly doesn't work anymore just because you didn't notice that it was getting too dark outside and the light isn't sufficient to power your device anymore.
To avoid this, either keep the solar shield at a fixed position and angle under an artificial light source or use a lab bench power supply as an emulated harvesting source.
For example, a 1V voltage fed via a 1kOhm resistor to the pin *Vin* of the Riotee Board makes for a nice steady harvesting input of around 160uW.

## Capacitor voltage monitoring

Application execution on the Riotee Board/Module is mainly dictated by the capacitor voltage.
If something does not immediately work as expected it is essential to be able to see the capacitor voltage.
To this end, connect an oscilloscope channel to the *Vcap* pin and watch the capacitor voltage during execution.
Ideally, use a mixed signal logic analyzer/oscilloscope to monitor capacitor voltage, UART output (on pin D1/TX) and any peripheral connections.
Keep in mind that, depending on the input impedance of your equipment, there may be significant current flowing into your device, changing execution flow.