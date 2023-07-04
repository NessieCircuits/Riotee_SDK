#include "riotee.h"
#include "riotee_i2c.h"

#include "Wire.h"

void RioteeI2C::begin() {
  tx_buffer_idx = 0;
  rx_buffer_idx = 0;
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
  unsigned int rx_space = I2C_BUFFER_SIZE - rx_buffer_idx;
  if (len > rx_space)
    len = rx_space;

  int rc = riotee_i2c_read(rx_buffer + rx_buffer_idx, len, address);
  if (rc != 0)
    return 0;

  rx_buffer_idx += len;
  return len;
}

int RioteeI2C::read(void) {
  if (rx_buffer_idx == 0)
    return -1;

  return rx_buffer[--rx_buffer_idx];
}

void RioteeI2C::beginTransmission(uint8_t address) {
  tx_address = address;
}

uint8_t RioteeI2C::endTransmission(void) {
  riotee_i2c_write(tx_address, tx_buffer, tx_buffer_idx);
  tx_buffer_idx = 0;
}

size_t RioteeI2C::write(uint8_t* data, size_t len) {
  unsigned int tx_space = I2C_BUFFER_SIZE - tx_buffer_idx;
  if (len > tx_space)
    len = tx_space;

  memcpy(tx_buffer + tx_buffer_idx, data, len);

  return len;
}

size_t RioteeI2C::write(uint8_t data) {
  if (tx_buffer_idx == I2C_BUFFER_SIZE)
    return 0;
  tx_buffer[tx_buffer_idx++] = data;
  return 1;
}