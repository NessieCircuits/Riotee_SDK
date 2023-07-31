(runtime_documentation)=
# Riotee Runtime

The Riotee runtime allows executing user-code on battery-free devices built around the Riotee Module.
Your code defines a `main()` function. This function is executed while the device has energy and suspended when energy becomes critically low.
If the power supply is interrupted, the stack and all static and global variables are stored in non-volatile memory and restored as soon as the supply comes back.

## Key concepts

### Task management

The runtime is based on FreeRTOS. This allows suspending and resuming execution of user code and facilitates a highly portable implementation of [*checkpointing*](checkpointing). This does not mean that user code can make use of FreeRTOS features such as multi-threading. Instead, user code is run in one dedicated task that is managed by the runtime.

### Capacitor voltage monitoring

The state of the device and the execution of code is mainly dictated by energy availability. The runtime continuously monitors the capacitor voltage with the two comparators on the Riotee module. The output of one of the comparators defines a *low* threshold. When the runtime detects this threshold it will halt execution and prepare the system for a potential power supply failure. The output of the second comparator defines a *high* threshold that indicates that the capacitor is sufficiently charged to start executing again. The voltage thresholds of these comparators can be set in discrete steps by the software.

(checkpointing)=
### Checkpointing

When the runtime detects a *low* capacitor voltage level, it suspends user code execution and puts the system to a deep sleep mode.
If the capacitor voltage does not recover above the *low* threshold within a short time, the runtime copies the complete state of the user task (stack, heap and registers) to non-volatile RAM.
There are two possible scenarios for what happens next: 1. The capacitor discharges further and the device loses power. This could be when, for example, someone covers the solar panel of a solar-harvesting device. Once power is restored, the device resets and the system task reloads the user application from non-volatile memory. The user task continues execution. 2. The capacitor voltage recovers above the *high* threshold. The runtime wakes up the user task and the application continues.

## Callbacks

There are a number of callbacks that your application can implement:

 - `bootstrap_callback()`: Called once after programming the device.
 - `startup_callback()`: Called right after every reset. Perform early stage initizialization required for low-power operation here.
 - `reset_callback()`: Called later after every reset. Initialize peripherals here.
 - `turnoff_callback()`: Called right before the application gets suspended. Abort any energy-intensive operation immediately.

## Retained memory

The runtime automatically checkpoints application state to non-volatile memory before an imminent power failure.
Unfortunately, this operation requires a lot of energy and happens when the energy stored in the capacitors is already low.
In order to guarantee the completion of the checkpoint operation, the amount of data that is to be stored must be small enough such that the energy required for storing is less than the usable energy in the capacitor.

To satisfy this condition, the amount of RAM that is available to the user is limited.
If you exceed the limit of (static) memory, the compilation will fail and you will receive an error message.

There are two things you can do to increase the memory your application can use.

1. Place your variables outside the retained memory area.

You can use the two macros `__NONRETAINED_INITIALIZED__` and `__NONRETAINED_ZEROED__` to declare variables as non-retained.
For example:

```C
static uint32_t myarr[1024] __NONRETAINED_ZEROED__;
```

defines an array that will be initialized to zero on every reset. The values stored in the array will **not** be retained across power failures.
You are responsible for keeping data and application flow consistent in this case.

2. Increase the retained memory area.

You may increase the memory area where retained variables are stored by defining `RIOTEE_RAM_RETAINED_SIZE` in your application's Makefile.
The SDK will reserve a memory area of the specified size for your variables and the runtime will automatically checkpoint all variables to non-volatile RAM.

The drawback is that, depending on the specified size, the checkpointing may take significantly longer and consume significantly more energy.
You must chose the capacitance of your device such that the checkpoint can still safely complete before the power supply gets interrupted.

The runtime only checkpoints the part of the retained memory area that is actually occupied by the variables and the stack so don't worry about reducing RIOTEE_RAM_RETAINED_SIZE beyond the default value of 8192B.
However you may find that you can fit more static/global variables into the retained memory when you reduce the (generous) default `RIOTEE_STACK_SIZE` in your application's Makefile.
For an example, take a look at the [dsp example's Makefile](https://github.com/NessieCircuits/Riotee_Runtime/blob/main/examples/dsp/Makefile).

## Early startup

When the capacitor is charged above a hardware-defined turn-on threshold, the power supply is switched on.
The early hardware initialization and current in-rush into the decoupling capacitors leads to a steep drop of the capacitor voltage.
The runtime thus first waits until the capacitor voltage has recovered before intitializing the system, calling the `reset` function and starting the user task.
If you have power-hungry peripherals that must be explicitly disabled or put into a low power mode, this can become a problem:
While the runtime waits until the capacitor voltage has recovered, your peripheral may consume more power than what is being harvested and the system never really starts up.

To prevent such situations, the runtime provides the `startup_callback()`.
This callback is executed almost immediately after reset and allows you to put any power-hungry peripherals into a low power mode such that the system can charge up.

Note that during the `startup_callback()`, the system is not yet setup and you cannot use static or global variables.
Really only do what's necessary to reduce the power consumption and do the remaining initialization later in the `reset_callback` when the capacitor has charged up sufficiently.


## Resources used by the runtime 

The SDK uses a number of peripherals and hardware resources on the nRF52. The user may not access these peripherals to avoid interference with the runtime and drivers:
 - Timer4 (core/nvm.c)
 - SPIM0 (core/spic.c)
 - SPIM3 (core/nvm.c)
 - TWIM1 (core/i2c.c)
 - UART0 (core/uart.c)
 - PPI
 - Timer2 (core/stella.c)
 - Radio (core/ble.c and core/stella.c)


## Peripheral drivers

One of the great challenges of operating battery-free devices is interaction with peripherals. Consider an application that is supposed to wait for a hardware timer to expire. The code sets up the timer and waits for a flag that is set from within an interrupt. While waiting, the power supply is interrupted and the device is reset. The timer is stopped and not trivially restored after a reset. The user task instead is restored in the loop waiting for the flag which will never be set. The system fails.

Another problem is the high power consumption of peripherals. For example, the radio consumes tens of milliwatts while transmitting a packet. This discharges the on-board capacitors within few milliseconds. If the power consumption is not minimized immediately after hitting the *low* threshold, there may not be sufficient energy to checkpoint the application state to non-volatile memory before the power supply finally fails.

To address these challenges, the runtime offers three peripheral driver models:

### Short operation

If the interaction with a peripheral is short and can be guaranteed to complete before the power supply fails (e.g. sending one character over UART), the interaction is implemented in a critical section, i.e. with interrupts disabled. This way, the user task cannot be swapped out while waiting for completion of the interaction. After leaving the critical section, any pending *low* threshold interrupts is served and execution can safely be stopped. An example for this type of driver is found in `src/uart.c`.

### Long-running operation

For longer running interactions the *critical section* approach is not feasible as it cannot be guaranteed that the interaction completes before power is interrupted.
Instead, a transaction is started and the user task blocks on a task notification, which is sent from the peripheral interrupt on completion. If the transaction completes, the notification is sent and the user task continues. If power fails and the device is reset, the runtime sends a different notification value to the user task. This way, the task is guaranteed to not block on the interaction indefinitely. By checking the notification value it can decide if the transaction has completed successfully or if it was interrupted by a reset. An example for this type of driver is found in `src/timing.c`.

### Power-intensive operation

If a peripheral consumes significant power, the previous approach can still lead to a failure of the runtime when the operation drains the capacitor before the state can be checkpointed.
Thus, for power-intensive operations, drivers must register a teardown function with the runtime.
Upon detecting a *low* threshold, the runtime calls the teardown function which aborts the operation to reduce the power consumption.
For this purpose, the `TEARDOWN_FUN` macro defined in `include/runtime.h` creates a function pointer variable that is placed in the `.teardown` section (see `linker.ld`).
When the peripheral is inactive, the variable is NULL.
After starting a power-intensive operation, the driver points this variable to its teardown function so the runtime can abort the operation if necessary.
The driver informs the application that the operation was interrupted by returning a corresponding return code.


## Walk-through

The runtime uses custom start-up code located in `src/startup.c`. The function `c_startup` is installed as the reset handler in the `vectors` interrupt vector table. First, it initializes the GPIOs to reduce leakage current on the pins shared with the MSP430. By default, the boost converter (MAX20361) shuts off the power supply when it detects below 3.7V on the capacitor. To prevent this, the startup code initializes I2C and disables this functionality. Next, the optional `startup_callback` is executed where a user may perform any early-stage initialization that must be done as soon after the reset as possible, for example to put an attached device to a low power mode.
Then the code waits until the *high* threshold is reached before initializing static/global variables (.data and .bss). Note that at this stage, only the system variables are initialized. User variables are stored in a separate section (`data_retained` and `bss_retained` in the linker script `linker.ld`) and initialized later in the process. After enabling the FPU, any static C++ constructors are initialized with a call to newlib's `__libc_init_array()`.

Next, the runtime is initialized and started in `runtime_start()` in `src/runtime.c`. After initializing UART, timing and gpio interrupt functionality, two FreeRTOS tasks are created and the FreeRTOS scheduler is started. The `sys_task` has higher priority so it executes first and immediately disables the `usr_task`. Just like the startup code it also waits for the capacitor voltage to be above the *high* threshold to make sure the system has enough energy for the next step. Next, the system task checks if this is a first boot-up after flashing the code. This is done by comparing a variable that was just copied over from flash against the known value (0x8BADF00D). Two possible outcomes: 1) If the value is correct, this is fresh boot and the code overwrites the corresponding value in flash with zeroes to guarantee that the condition is false after the next reset. Next, static/global user variables are initialized, just like we did for system variables in `c_startup`. The system calls the `bootstrap_callback` where a user may perform one-time initialization. 2) This is not a fresh boot. The system task loads the previous application state from non-volatile memory into the stack structure and into the `data_retained` and `bss_retained` memory sections (see `checkpoint_load(..)`).

Next, the system task calls `reset_callback` where a user may perform hardware initialization necessary after every reset. The system task resumes the user task (does not run, yet because of lower priority) installs a handler to react to a *low* threshold and blocks for the corresponding notification. At this point, the user task becomes the highest priority task that is able to run and gets swapped in. It executes until the *low* threshold handler is called. The handler wakes up the system task which suspends the user task and executes the `teardown` function.
This function iterates a table of function pointers where device drivers can register functions that abort any power-intensive process like transmitting a packet and inform the application that the operation has failed.

Next, the system task registers a callback for detection of a *high* threshold. If the application was already waiting on a high threshold, it is overridden as it will anyways only continue execution after the *high* threshold is reached again.
The system task starts a 10ms timer and transitions to sleep mode.
If the harvested power is much greater than the sleep power, the system recovers above the *high* threshold before the timer expires. The timer gets stopped and the execution of user code continues.
If the capacitor voltage is still below the *low* threshold when the timer expires, a snapshot of the application data is stored to non-volatile RAM (`checkpoint_write(..)`).
If the capacitor voltage is above the *low* threshold when the timer expires, the system task registers a callback for detection of a *low* threshold.
If the capacitor voltage falls below the *low* threshold again, a snapshot is taken and the system task waits until the *high* threshold is reached.
If the capacitor voltage recovers above the *high* threshold, the user task is resumed and execution continues.
If power supply fails, the system starts from `c_startup` again, restores the user task and continues execution.


## API reference

```{eval-rst}
.. doxygengroup:: riotee
   :project: riotee
   :content-only:
```