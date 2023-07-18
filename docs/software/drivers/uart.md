# UART


The UART driver currently supports one TX-only UART instance mapped to pin D1/TX with a configurable baudrate.

Note that the UART is clocked from a rather inaccurate RC-oscillator.
Therefore, baudrates may differ significantly from the specified value.

## Example usage

```{eval-rst}
.. literalinclude:: ../../../examples/uart/src/main.c
   :language: c
   :linenos:
```

## API reference

```{eval-rst}
.. doxygengroup:: uart
   :project: riotee
   :content-only:
```