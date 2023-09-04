# Riotee Arduino Core


[![Tests](https://github.com/NessieCircuits/Riotee_ArduinoCore/actions/workflows/test.yml/badge.svg)](https://github.com/NessieCircuits/Riotee_ArduinoCore/actions/workflows/test.yml)

This repository hosts the runtime and Arduino bindings for the battery-free Riotee devices. After installation you can program the Riotee module or Riotee board conveniently from the Arduino IDE via a Riotee probe (already built into the Riotee board).

# Installation

 - Open the board manager in the Arduino IDE by clicking on the second symbol from the top on the side panel.
 - Navigate to `File->Preferences` in the menu bar.
 - Open the list of board managers by clicking on the icon next to the input field for *Additional boards manager URLs*.
 - Add `https://riotee.nessie-circuits.de/arduino/package_riotee_index.json` as one new row to the list of board manager URLs.
 - Confirm the changes by clicking OK.
 - Look for "Riotee Boards by Nessie Circuits" in the board manager side panel and click Install.

# Usage

Select the Riotee board in the Arduino IDE under `Tools->Board->Riotee Boards->Riotee Board`.
