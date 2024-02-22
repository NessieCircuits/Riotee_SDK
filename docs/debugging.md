# Debugging

Although the Riotee SDK takes much of the pain of developing battery-free applications away, it remains much more challenging than developing for battery-powered devices.
An effective development setup is therefore essential.

## Constant harvesting source

While developing, a reliable, constant energy input helps to keep results comparable.
Nothing is more frustrating than hunting a phantom bug for hours when your application suddenly doesn't work anymore just because you didn't notice that it was getting too dark outside and the light isn't sufficient to power your device anymore.
To avoid this, either keep the solar shield at a fixed position and angle under an artificial light source or use a lab bench power supply as an emulated harvesting source.
For example, a 1V voltage fed via a 1kOhm resistor to the pin *Vin* of the Riotee Board provides a steady harvesting input of around 160uW.

## Capacitor voltage monitoring

Application execution on the Riotee Board/Module is mainly dictated by the capacitor voltage.
If something does not immediately work as expected it is essential to be able to see the capacitor voltage.
To this end, connect an oscilloscope channel to the *Vcap* pin and watch the capacitor voltage during execution.
Ideally, use a mixed signal logic analyzer/oscilloscope to monitor capacitor voltage, UART output (on pin D1/TX) and any peripheral connections.
Keep in mind that, depending on the input impedance of your equipment, there may be significant current flowing into your device, changing execution flow.

## Constant power supply

For initial development of new features it is often helpful to run Riotee in a continuously powered mode like any standard development kit.
For this purpose, the Riotee Probe, which is also present on the Riotee Board, allows powering the Riotee Module with a [constant power supply](constant_power_supply).

:::{important}
:name: disable_cap_monitor

At multiple instances, the Riotee runtime waits for the capacitor voltage to reach a threshold before continuing execution. Similarly, many applications use the `riotee_wait_cap_charged()` call to wait for this threshold. As a result, even when you have the constant power supply enabled, your application will not run when you don't have a harvester attached. To allow your application to execute regardless, you can compile it with the flag `DISABLE_CAP_MONITOR` defined. This flag disables all waiting for the capacitor voltage threshold and allows code to execute while the constant power supply is enabled and no harvester is attached.

To compile your application with the `DISABLE_CAP_MONITOR` flag, call `make USER_DEFINES=-DDISABLE_CAP_MONITOR`.
:::


## GDB Debugging

The RP2040 microcontroller on the Riotee Board and Riotee Probe exposes a CMSIS-DAP interface for debugging via gdb.

:::{important}
When entering a debugging session, the Riotee Probe automatically enables the [constant power supply](constant_power_supply). However, as long as you don't have a harvester attached, you still need to compile your application with the `DISABLE_CAP_MONITOR` flag defined. See [the hint above](#disable_cap_monitor).
:::

### Command-line

After installing *pyOCD* with `pip install pyocd`, you can start a gdb server with `pyocd gdbserver --elf _build/build.elf --target nrf52832` and connect to the session with your favorite debugging tool.


### Visual Studio Code

For Visual Studio Code, install the *Cortex-Debug* extension from the marketplace and place a `launch.json` in the `.vscode` directory of your project with the following content:
```json
{
    "configurations": [
        {
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceRoot}/_build/build.elf",
            "name": "Debug with PyOCD",
            "request": "launch",
            "type": "cortex-debug",
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none",
            "servertype": "pyocd"
        },
    ]
}
```

Select *Run and Debug* from the side panel and click the green play button in the upper left bar.


