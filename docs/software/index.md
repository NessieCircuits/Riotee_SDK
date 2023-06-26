# Software Development Kit

## Callbacks

## Retained memory

The runtime automatically checkpoints application state to non-volatile memory before an imminent power failure.
Unfortunately, this operation requires a lot of energy and happens when the energy stored in the capacitors is already low.
In order to guarantee the completion of the checkpoint operation, the amount of data that is to be stored must be small enough such that the energy required for storing is less than the usable energy in the capacitor.

To satisfy this condition, the amount of RAM that is available to the user is limited.
If you exceed the limit of (static) memory, the compilation will fail and you will receive an error message.

There are two things you can do to increase the memory your application can use.

1. Place your variables outside the retained memory area.

You can use the two macros `__VOLATILE_UNINITALIZED` and `__VOLATILE_INITALIZED` to declare variables as non-retained.
For example:

```C
static uint32_t myarr[1024] __VOLATILE_UNINITALIZED;
```
defines an array that will be initialized to zero on every reset. The values stored in the array will **not** be retained across power failures.
You are responsible for keeping data and application flow consistent in this case.

2. Increase the retained memory area.

You may increase the memory area where retained variables are stored by defining a constant `RIOTEE_RAM_RETAINED_SIZE` in a header file `riotee_config.h` that's on one of your include paths.
The SDK will reserve a memory area of the specifed size for your variables and the runtime will automatically checkpoint all variables to non-volatile RAM.
The drawback is that, depending on the specified size, the checkpointing may take significantly longer and consume significantly more energy.
You must chose the capacitance of your device such that the checkpoint can still safely complete before the power supply gets interrupted.


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

## Teardown


## Developer docs

### Driver models

### Linker script generation