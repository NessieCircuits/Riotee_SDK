#include "nrf.h"
#include "nrf_gpio.h"

#include "Arduino.h"

#define PINS_COUNT 64

void pinMode(uint32_t ulPin, uint32_t ulMode) {
  if (ulPin >= PINS_COUNT) {
    return;
  }

  NRF_GPIO_Type *port = nrf_gpio_pin_port_decode(&ulPin);

  // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
  switch (ulMode) {
  case INPUT:
    // Set pin to input mode
    port->PIN_CNF[ulPin] =
        ((uint32_t)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
        ((uint32_t)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        ((uint32_t)GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
        ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
    break;

  case INPUT_PULLUP:
    // Set pin to input mode with pull-up resistor enabled
    port->PIN_CNF[ulPin] =
        ((uint32_t)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
        ((uint32_t)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        ((uint32_t)GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
        ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
    break;

  case INPUT_PULLDOWN:
    // Set pin to input mode with pull-down resistor enabled
    port->PIN_CNF[ulPin] =
        ((uint32_t)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
        ((uint32_t)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        ((uint32_t)GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos) |
        ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
    break;

  case OUTPUT:
    // Set pin to output mode
    port->PIN_CNF[ulPin] =
        ((uint32_t)GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
        ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
        ((uint32_t)GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
        ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
    break;

  default:
    // do nothing
    break;
  }
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal) {
  if (ulPin >= PINS_COUNT) {
    return;
  }

  NRF_GPIO_Type *port = nrf_gpio_pin_port_decode(&ulPin);

  switch (ulVal) {
  case LOW:
    port->OUTCLR = (1 << ulPin);
    break;

  default:
    port->OUTSET = (1 << ulPin);
    break;
  }

  return;
}