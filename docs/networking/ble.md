# Bluetooth Low Energy

Riotee has limited support for BLE. You can use the API to send advertisement packets with a custom payload on the three advertising channels.

The implementation is not standard-conformant. You may only use it for evaluation purposes in dedicated facilities where it cannot interfere with other spectrum users.

## Example usage

```{eval-rst}
.. literalinclude:: ../../examples/ble/src/main.c
   :language: c
   :linenos:
```

## API reference

```{eval-rst}
.. doxygengroup:: ble
   :project: riotee
   :content-only:
```