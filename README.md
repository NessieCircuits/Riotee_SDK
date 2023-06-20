[![Build](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/build.yml/badge.svg)](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/build.yml)

# Riotee Software Development Kit

The code in this repository allows building applications that run on battery-free Riotee devices.
For a detailed description refer to the [documentation](https://www.riotee.nessie-circuits.de/docs/software/riotee-runtime).

For a quickstart with Riotee you can use our Arduino package which internally makes use of the SDK. For more involved projects, continue reading here.

## Installation

Download the [latest zip release](https://github.com/NessieCircuits/Riotee_Runtime/releases/latest) and unpack it to a path on your machine.

Alternatively, clone this repository to get the latest development version of the SDK:

```bash
git clone --recursive git@github.com:NessieCircuits/Riotee_Runtime.git
```

### Linux

 - Install `make` from your distribution's repository (`apt install build-essential` on Ubuntu)
 - Install the `GNU Arm Embedded Toolchain` from your distribution's repository (`apt install gcc-arm-none-eabi` on Ubuntu) or from [the official website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
 - For uploading firmware to your Riotee device install [pyOCD](https://pyocd.io/) with

```bash
pip install pyocd
```

### Windows
 - Install `make` by downloading and running [the Cygwin installer](https://cygwin.com/). Be sure to select `make` during the installation.
 - Add `C:/cygwin64/bin` to your `Path` variable ([Instructions](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)).
 - Install the `GNU Arm Embedded Toolchain` from the [official website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).
 - For uploading firmware to your Riotee device install [pyOCD](https://pyocd.io/) with

```bash
pip install pyocd
```
## Usage

Take a look at the [examples](./examples) for how you can use this SDK. We also provide a [template project](https://github.com/NessieCircuits/Riotee_AppTemplate) to get you started with your first Riotee project using the SDK.

In a nutshell: Your application provides a `main()` function. This function is executed while the device has energy and suspended when energy becomes critically low. If the power supply is interrupted, the stack and all static and global variables are stored in non-volatile memory and restored as soon as the supply comes back.

There are a number of callbacks that your application can implement:
 - `startup_callback()`: Called right after every reset. Perform early stage initizialization required for low-power operation here
 - `reset_callback()`: Called later after every reset. Initialize peripherals here.
 - `turnoff_callback()`: Called right before the application gets suspended. Abort any energy-intensive operation immediately.

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
 - *Stella* wireless protocol for bidirectional communication with a basestation

## Code structure

 - `/core`: Riotee runtime and peripheral drivers
 - `/drivers`: Drivers for external devices
 - `/examples`: Examples using the SDK
 - `/external`: External dependencies
