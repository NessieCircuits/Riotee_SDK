#include "riotee.h"
#include "riotee_i2c.h"
#include "printf.h"

#include "Wire.h"

void RioteeI2C::begin() {
  tx_ringbuf.clear();
  rx_ringbuf.clear();
  return;
}

void RioteeI2C::begin(uint8_t address) {
  /* Peripheral mode not implemented. */
  return;
}

void RioteeI2C::setClock(uint32_t freq) {
  switch (freq) {
    case 100000:
      riotee_i2c_set_freq(RIOTEE_I2C_FREQ_100K);
      return;
    case 250000:
      riotee_i2c_set_freq(RIOTEE_I2C_FREQ_250K);
      return;
    case 400000:
      riotee_i2c_set_freq(RIOTEE_I2C_FREQ_400K);
      return;
    default:
      /* The API definition forces this function to be void so we can't return an error. */
      return;
  }
}
size_t RioteeI2C::requestFrom(uint8_t address, size_t len) {
  uint8_t rx_buf_tmp[I2C_BUFFER_SIZE];
  if (len > rx_ringbuf.availableForStore())
    len = rx_ringbuf.availableForStore();

  if (riotee_i2c_read(rx_buf_tmp, len, address) != RIOTEE_SUCCESS)
    return 0;

  for (unsigned int i = 0; i < len; i++)
    rx_ringbuf.store_char(rx_buf_tmp[i]);

  return len;
}

size_t RioteeI2C::requestFrom(uint8_t address, size_t len, bool stopBit) {
  /* Peripheral mode not implemented. */
  return 0xFFFFFFFF;
}

int RioteeI2C::read(void) {
  return rx_ringbuf.read_char();
}

void RioteeI2C::beginTransmission(uint8_t address) {
  tx_address = address;
}

uint8_t RioteeI2C::endTransmission(void) {
  size_t tx_size = tx_ringbuf.available();
  for (int i = 0; i < tx_size; i++) {
    uint8_t data = tx_ringbuf.read_char();
    if (riotee_i2c_write(tx_address, &data, 1) != RIOTEE_SUCCESS)
      return i;
  }
  return tx_size;
}

uint8_t RioteeI2C::endTransmission(bool stopBit) {
  /** Peripheral mode not implemented. */
  return 0xFF;
}

size_t RioteeI2C::write(uint8_t* data, size_t len) {
  size_t tx_space = tx_ringbuf.availableForStore();
  if (len > tx_space)
    len = tx_space;

  for (int i = 0; i < len; i++)
    tx_ringbuf.store_char(data[i]);

  return len;
}

size_t RioteeI2C::write(uint8_t data) {
  if (tx_ringbuf.availableForStore() == 0)
    return 0;
  tx_ringbuf.store_char(data);
  return 1;
}

int RioteeI2C::available(void) {
  return rx_ringbuf.available();
}

int RioteeI2C::peek(void) {
  return rx_ringbuf.peek();
}

RioteeI2C Wire;
