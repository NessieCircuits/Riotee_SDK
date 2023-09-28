# I2C

I2C Controller driver.
The driver supports one instance of a I2C controller with fixed pin configuration and configurable datarate.

A call to `riotee_i2c_init()` initializes the controller with a rate of 250kbps.

Two sets of transfer functions are provided.
The `riotee_i2c_x_atomic(...)` API encapsulates reads and writes in a critical section and works even before the Riotee runtime is initialized.
It is used to initialize the on-board peripherals during early startup.

The `riotee_i2c_x(...)` should be used in all other cases.

:::{important}
Always check the return code of `riotee_i2c_x(...)` to check for a potential reset/teardown during the transfer.
:::

## Example usage

```{eval-rst}
.. literalinclude:: ../../../examples/i2c/src/main.c
   :language: c
   :linenos:
```

## API Reference

```{eval-rst}
.. doxygengroup:: i2c
   :project: riotee
   :content-only:
```