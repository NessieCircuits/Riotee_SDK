# Quickstart

Just got yourself a Riotee board? Welcome to the battery-free Internet of Things!

To follow this guide, you will need;

 - Riotee Board
 - Riotee Solar Shield
 - USB-C cable
 - PC with the Arduino IDE installed

## Prepare the Riotee board

If you haven't done it already, solder the pin headers to the Riotee Board and the Solar Shield.

:::{caution}
  Pay close attention to the orientation and the mounting angle when soldering the headers. It is difficult to de-solder the headers once they're soldered on to more than one pad.
:::

Stack the Solar Shield into the mating headers of the Riotee Board.

(arduino_install)=
## Install the Riotee Arduino Core

 - Open the Arduino IDE and navigate to `File->Preferences`.
 - Open the list of board managers by clicking on the icon next to the input field for *Additional boards manager URLs*.
 - Add `https://riotee.nessie-circuits.de/arduino/package_index.json` as one new row to the list of board manager URLs.
 - Confirm the changes by clicking OK.

## Linux only: Install the udev rules

 - Download the [Riotee udev rules](https://github.com/NessieCircuits/Riotee_ProbeSoftware/blob/main/52-riotee.rules) and copy them to the `/etc/udev/rules.d/` directory on your machine.
 - Make sure your user belongs to the *plugdev* group and reload the udev rules with:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Write your Arduino sketch

Don't know what to write, yet? Copy this example into your sketch:

```C

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  riotee_wait_cap_charged();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
}

```

## Upload your code

1. Connect the Riotee Board to your PC with the USB-C cable
2. Select the Riotee board in the Arduino IDE under `Tools->Board->Riotee Boards->Riotee Board`
3. Click the `Upload` in the toolbar

## Test your battery-free device

Put both sliders on the DIP switch on top of the solar shield to the position close to the text "ON ADE02S". In this configuration, all four panels provide power. Watch the red LED on the Riotee Module right next to the metal shield. Does your device start blinking, yet? If not, it may be too dark. This is bad for your eyes and even worse for a solar-powered device! Unplug the USB cable and take the device to a brighter location. This can be close to a lamp or anywhere outside. You should now see your device blink with a frequency depending on the incident light.

Congratulations, you've just built your first battery-free device!
