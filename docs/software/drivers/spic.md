# SPIC

SPI Controller driver.
The driver supports one instance of a 4-wire SPI controller.
Any of the 11 GPIOs can be mapped to any of the four signals.

:::{important}
Always check the return code of `riotee_spic_transfer(...)` to check for a potential reset/teardown during the transfer.
:::

## Example usage

```{eval-rst}
.. literalinclude:: ../../../examples/spic/src/main.c
   :language: c
   :linenos:
```

## API Reference

```{eval-rst}
.. doxygengroup:: spic
   :project: riotee
   :content-only:
```