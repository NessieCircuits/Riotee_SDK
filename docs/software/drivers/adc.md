# ADC


The Riotee Module has two analog pins that can be sampled with the 12-bit successive approximation ADC on-board the nRF52.
Additionally, the ADC can be used to sample the supply voltage and the capacitor voltage.

The API supports simple reading of single samples with a default configuration and more sophisticated periodic sampling with custom configuration.

:::{important}
Always check the return code of `riotee_adc_sample(...)` to ensure that sampling has actually completed (`RIOTEE_SUCCESS`) before working with the data.
:::

## Example usage

```{eval-rst}
.. literalinclude:: ../../../examples/adc/src/main.c
   :language: c
   :linenos:
```

## API reference

```{eval-rst}
.. doxygengroup:: adc
   :project: riotee
   :content-only:
```