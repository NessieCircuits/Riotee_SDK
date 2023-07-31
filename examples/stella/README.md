# Stella Example

The example sends a packet containing a 32-bit counter over the wireless radio using the *Stella* protocol.
To test the example, you'll need to setup a [Stella Gateway](https://github.com/NessieCircuits/Riotee_Gateway) following the instruction in the Readme.

After flashing the example and setting up the gateway, call

```bash
riotee-gateway client monitor -d [DEVICE_ID]
```

and watch the received packets on your console.