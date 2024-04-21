# RTC example

Demonstrates basic functionality and backup supply mode of the AM1805 RTC.

The example enables trickle charging of the backup capacitors of the RTC. After a short time, the capacitors should be charged to around 1.7V. Observe the terminal output while disconnecting the energy harvesting source. Wait for a minute or two and reconnect the harvester. The displayed time should be the time when you cut power plus the duration of the power outage.