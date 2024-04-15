# Current consumption

To make the most out of the harvested energy, the Riotee Module is designed for low current consumption.

There are three main modes with distinct current consumption figures.
In the *active* mode, the current consumption depends on the used peripherals and the state of the CPU. For example, the nRF52833 draws around 4.7mA when the CPU is active.
When the 2.4GHz wireless radio is listening for incoming packets in RX mode, it draws 9.6mA.

While waiting for the capacitor voltage to recharge and reach the turn-on threshold, the Riotee module consumes a total of around 4.5uA.
This includes the current consumption for the comparators, the MSP430 in sleep mode, the AM1805 RTC and the nRF52833 with an active 32kHz crystal oscillator.

The following table lists the current consumption with respect to the capacitor voltage:

| Mode       | Current Consumption |
|------------|---------------------|
| RTC-only   | 45nA                |
| Recharging | 4.5uA               |
| Active     | >4.5uA              |

The capacitor discharge current is between 60nA and 120nA higher than the values in the table as it also includes the quiescent current of the 2V regulator.

(measuring_current)=
## Measuring current consumption

The Riotee Module and Riotee Board allow measuring the current consumption drawn from the 2V supply by rerouting the current through a [power switch](power_switches).

Due to the rapid changes between different operating states, the current consumption of the Riotee Module is highly dynamic.
Make sure to use the right instrument and configure it for the corresponding measurement and sampling rate.

### Riotee Module

Setting high pin *Bypass* activates a bypass switch and reroutes the current between *+2V (Aon)* and *Vshunt* on the Riotee Module.
Connect an amperemeter between *+2V (Aon)* and *Vshunt* to measure the current consumption on the 2V supply.

### Riotee Board

On the Riotee Board, the bypass switch can be activated with the `riotee-probe` command line utility.
Connect an amperemeter between *+2V (Aon)* and *+2V* to measure the current consumption on the 2V supply.
Note that with this measurement setup, the AM1805 on-board the Riotee Module is not supplied with power.