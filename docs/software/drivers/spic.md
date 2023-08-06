# SPIC

SPI Controller driver.
The driver supports one instance of a 4-wire SPI controller, all four SPI modes and configuration of the bit order (MSB first or LSB first).
Any of the 11 GPIOs can be mapped to any of the four signals.
The *Chip Select* line is controlled automatically by the driver.
By setting *riotee_spic_cfg_t.pin_cs* to `RIOTEE_SPIC_PIN_UNUSED`, this functionality is disabled, allowing manual control of the *Chip Select*.

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