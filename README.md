# A battery-free runtime for the Riotee platform

The code in this repository allows executing user-code on the battery-free Riotee module. 

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

## Key concepts

### Task management

The runtime is based on FreeRTOS. This allows to easily suspend and resume execution of user code and facilitates a highly portable implementation of *checkpointing*. This does not mean that user code can make use of FreeRTOS features such as multi-threading. Instead, user code is run in one dedicated task that is managed by the runtime.

### Capacitor voltage monitoring

The state of the device and the execution of code is mainly dictated by energy availability. The runtime continuously monitors capacitor voltage with the two comparators on the Riotee module. The output of one of the comparators defines a *low* threshold. When the runtime detects this threshold it will halt execution and prepare the system for a potential power supply failure. The output of the second comparator defines a *high* threshold that indicates that the capacitor is sufficiently charged to start executing again. The voltage thresholds of these comparators can be set in discrete steps by the software.

### Checkpointing

When the runtime detects a *low* capacitor voltage level, it stops code execution and copies the complete state of the user task (stack, heap and registers) to non-volatile RAM and puts the CPU to sleep. There are two possible scenarios for what happens next: 1) The capacitor discharges further and the device loses power. This could be when, for example, someone covers the solar panel of a solar-harvesting device. Once power is restored, the device resets and the system task reloads the user application from non-volatile memory. The user task continues execution. 2) The capacitor voltage recovers above the *high* threshold. The system task yields and the user task continues execution.

## Peripherals

One of the greatest challenges of operating battery-free devices is interaction with peripherals. Consider an application that is supposed to wait for a hardware timer to expire. The code sets up the timer and waits for a flag that is set from within an interrupt. While waiting, the power supply is interrupted and the device is reset. The timer is stopped and not trivially restored after a reset. The user task instead is restored in the loop waiting for the flag which will never be set. The system fails.

To avoid this, our runtime has two ways to interact with peripherals: If the interaction is short and can be guaranteed to complete before the power supply fails (e.g. sending a character over UART), the interaction is implemented in a critical section, i.e. with interrupts disabled. This way, the user task cannot be swapped out while waiting for completion of the interaction. After leaving the critical section, any pending *low* threshold interrupts is served and execution can safely be stopped. Here's an example from our UART driver:

```
  taskENTER_CRITICAL();
  NRF_UART0->TXD = character;
  NRF_UART0->TASKS_STARTTX = 1UL;
  while (NRF_UART0->EVENTS_TXDRDY == 0) {
  };
  NRF_UART0->EVENTS_TXDRDY = 0;
  NRF_UART0->TASKS_STOPTX = 1UL;
  taskEXIT_CRITICAL();
  ```

For longer running interactions this approach is not feasible as it cannot be guaranteed that the interaction completes before power is cut. Instead, a transaction is started and the user task blocks on a task notification, which is sent from the peripheral interrupt on completion. If the transaction completes, the notification is sent and the user task continues. If power fails and the device is reset, the runtime sends a different notification value to the user task. This way, the task is guaranteed to not block on the interaction indefinitely. By checking the notification value it can decide if the transaction has completed successfully or if it was interrupted by a reset.

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

## Walk-through

The runtime uses custom start-up code located in `src/startup.c`. The function `c_startup()` is installed as the reset handler in the `vectors` interrupt vector table. First, the GPIOs are initialized to a defined state to reduce leakage current on the pins shared with the MSP430FR. By default, the boost converter (MAX20361) shuts off the power supply when it detects below 3.7V on the capacitor. To prevent this, the startup code initializes I2C and disables this functionality. Next, the code waits until the *high* threshold is reached before initializing static/global variables (.data and .bss). Note that at this stage, only the system variables are initialized. User variables are stored in a separate section (`data_retained` and `bss_retained` in the linker script `linker.ld`) and initialized later in the process. After enabling the FPU on-demand, static C++ constructors are initialized with a call to newlib's `__libc_init_array()`.

Next, the runtime is initialized and started in `runtime_start()` in `src/runtime.c`. After initializing UART, timing and gpio interrupt functionality, two FreeRTOS tasks are created and the FreeRTOS scheduler is started. The `sys_task` has higher priority so it executes first and immediately disables the `usr_task`. Just like the startup code it also waits for the capacitor voltage to be above the *high* threshold to make sure the system has enough energy for the next step. Next, the system task checks if this is a first boot-up after flashing the code by comparing a variable that was just copied over from flash against the known value (0x8BADF00D). Two possible outcomes: 1) If the value is correct, this is a fresh boot and the code overwrites the corresponding value in flash with zeroes to make sure that the condition is false after the next reset. Next, static/global user variables are initialized, just like we did for system variables in `c_startup`. The system calls the `bootstrap_callback()` where a user may perform one-time initializations. 2) This is not a fresh boot. The system task loads the previous application state from non-volatile memory into the stack structure and into the `data_retained` and `bss_retained` memory sections in `taskstore_get(..)`.

Next, the system task calls the `reset_callback()` where a user may perform hardware initialization necessary after every reset. The system task resumes the user task (does not run, yet because of lower priority), installs a handler to react to a *low* threshold and blocks for the corresponding notification. At this point, the user task becomes the highest priority task that is able to run and gets swapped in. It executes until the *low* threshold handler is called. The handler wakes up the system task which again suspends the user task and copies a snapshot of the application data to non-volatile RAM in `taskstore_write(..)`. Next, the system task installs a handler for the *high* threshold and blocks until the capacitor voltage recovers. At this time, no task is ready to run and FreeRTOS swaps in the idle task which puts the system into low power mode (see `vApplicationIdleHook()`). If the capacitor voltage recovers, the user task is resumed and the application continues. If power supply fails, the system starts from `c_startup()` again, restores the user task and continues execution.