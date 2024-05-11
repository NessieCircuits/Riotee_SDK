# Microspeech Example

Demonstrates usage of VM1010 microphone and TensorFlow Light for Microcontrollers library.

You'll need a Riotee Board, a Sensor Shield, a Capacitor Shield and an energy harvester. The example should work with all MLCCs on the Capacitor Shield activated, i.e. 3x47uF and 3x220uF.

Our application listens for a sound with the microphone in a low-power mode. Upon detecting a sound, it captures a 1s audio sample with a sampling rate of 16kHz. Subsequently, the audio sample undergoes preprocessing and is then analyzed by a machine learning model designed to recognize and differentiate between two predefined hotwords (yes/no). The classification outcome is transmitted to a smartphone via BLE.