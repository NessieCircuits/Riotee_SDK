# Basics

The energy harvested by a battery-free device is often unreliable and insufficient to power the device directly.
For example, a tiny solar panel may deliver only 100uW of power, while a typical low power device consumes tens of milliwatts when active.
To remain operable, the device accumulates energy in tiny, sustainable capacitors.
When those capacitors are charged, the device will activate and function until its capacitors are drained, at which point it will turn off until it has gathered enough energy to resume operation.
This switching between on and off states can happen multiple times per second.

```{image} img/intermittent_execution.svg
:width: 100%
```

## Intermittent execution

This *intermittent operation* makes it extremely difficult to run meaningful applications on a battery-free device;
Every time the device turns off, the contents of the RAM and the CPU registers are lost.
When power returns, the device resets and the application starts all over.
This prevents running long-running tasks like machine-learning inference on battery-free devices.

Riotee's hardware-and-software solution automatically handles this *intermittent execution* so you can focus on building your application.
To this end, Riotee continuously monitors the capacitor voltage, and gracefully suspends execution and puts the device into a low power sleep mode when the voltage falls below a software-defined turn-off threshold.
When the voltage recovers above another software-defined turn-on threshold, execution continues.
If the voltage does not recover within a short time, the complete application state is copied (*checkpointed*) to a non-volatile memory.
Later, when power becomes available again, this *checkpoint* is restored and execution continues.

## Inconsistencies

This *checkpointing* can lead to inconsistent system states when the application's state is restored but the (volatile) peripherals are not.
Consider an application that is supposed to wait for a hardware timer to expire.
The code sets up the timer and waits for a flag that is set from within an interrupt. While waiting, the power supply is interrupted and the device is reset.
The timer is stopped and not trivially restored after a reset.
The application instead is restored in the loop waiting for the flag which will never be set.
The system fails.
The Riotee runtime implements a driver model that is able to detect and handle such inconsistencies.
Head over to the [runtime documentation](runtime_documentation) to learn more about this.

## Timekeeping

Time plays an essential role in many embedded applications.
A sense of time is necessary to schedule communication or to timestamp sensor readings.
Battery-powered devices usually use a low-power RTC to keep track of time across days, months and years.
But such RTC also loses its internal counter value as soon as the power supply turns off.

To address this challenge, the Riotee Module has an ultra-low power RTC with additional backup capacitors.
When the supply voltage fails, these capacitors can supply the RTC for multiple minutes during which it keeps counting up its internal timers.
