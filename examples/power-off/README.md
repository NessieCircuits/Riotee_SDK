# RTC Power-off example

Uses the AM1805 RTC and the built-in p-MOS power switch to power down the system for a defined time (15s in this example). All parts of the system except for the RTC are powered off completely, reducing the current consumption to around 45nA. After 15s, power is restored, the microcontrollers are reset and execution continues after the explicit checkpoint.