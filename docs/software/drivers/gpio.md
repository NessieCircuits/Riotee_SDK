# GPIO

The Riotee Module/Board has 11 digital GPIO pins.
The API allows configuring pins as input or output.
Outputs can be controlled similar to any other battery-powered device.
Inputs can always be read with `riotee_gpio_read(unsigned int pin)` or the application can wait for a specified level on the pin with `riotee_gpio_wait_level(...)`.
Always check the return code of `riotee_gpio_wait_level(...)` to distinguish between a power failure (RIOTEE_ERR_RESET) and an actual level detection event (RIOTEE_SUCCESS).

## API Reference

```{eval-rst}
.. doxygengroup:: gpio
   :project: riotee
   :content-only:
```