# VM1010 Microphone Example

The example demonstrates *wake on sound* and audio sampling with the VM1010 analog microphone on-board the Riotee Sensor Shield.
It enables the microphone and waits for a sound level above a hardware defined threshold.
Upon detection of the sound it takes 120 audio samples at a sampling frequency of 4kHz and sends the recorded snippet to a basestation via the [Stella protocol](https://www.riotee.nessie-circuits.de/docs/latest/networking/stella.html).

To run the example, you will need a Riotee Board, a Riotee Sensor Shield and a power source like the Riotee Solar Shield.
To also receive the sampled audio, you'll need to setup a [Riotee Gateway](https://www.riotee.nessie-circuits.de/docs/latest/networking/stella.html#riotee-gateway).