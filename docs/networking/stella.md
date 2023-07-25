# Stella protocol

We provide a custom network protocol called *Stella* that allows Riotee devices to exchange data bi-directionally with a basestation.

The connection-less protocol uses the 2.4GHz radio on the Riotee devices and is based on the BLE 1Mbit phsyical layer.
Communication takes place on one channel and is device-initiated:
At any time, a device sends a packet to the basestation containing a device ID, a packet ID, an optional acknowledgment of previously received packet and a payload of a maximum of 247 Byte.
After transmitting the packet, the device transitions into RX mode and listens for a response from the basestation.
The basestation continuously listens for incoming packets.
Upon reception of a packet from a device, the basestation responds with an acknowledgment packet containing the device's ID, the packet ID of the received packet that is being acknowledged, the packet ID of the current packet and a payload with a maximum size of 247 Byte.

:::{important}
   Always check the return code of `riotee_stella_send(...)` and `riotee_stella_transceive(...)` to distinguish between a power failure (RIOTEE_ERR_RESET)and  successful communication (RIOTEE_SUCCESS).
:::


## Riotee Gateway

We provide a reference implementation for a basestation using a Nordic Semiconductor nRF52840-Dongle [here](https://github.com/NessieCircuits/Riotee_Gateway).
The Gateway forwards messages received from devices to a host application running on a user's computer via an HTTP API.
It also allows sending messages to devices.

## Example usage

```{eval-rst}
.. literalinclude:: ../../examples/stella/src/main.c
   :language: c
   :linenos:
```

## API reference

```{eval-rst}
.. doxygengroup:: stella
   :project: riotee
   :content-only:
```