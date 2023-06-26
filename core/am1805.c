#include "riotee_am1805.h"
#include "riotee_i2c.h"
#include "riotee_timing.h"
#include <string.h>

#define TX_BUF_SIZE 64
#define N_RETRIES 20
#define RETRY_DELAY_MS 20

uint8_t tx_buf[TX_BUF_SIZE];

static uint8_t dec2hex(uint8_t dec) {
  return ((dec / 10) << 4) + (dec % 10);
}

static uint8_t hex2dec(uint8_t hex) {
  return ((hex >> 4) * 10) + (hex & 0xF);
}

static int read_registers(uint8_t* dst, size_t n_data, uint8_t reg_addr) {
  int rc;
  if ((rc = riotee_i2c_write(AM1805_I2C_ADDR, &reg_addr, 1)) != 0)
    return rc;
  return riotee_i2c_read(dst, n_data, AM1805_I2C_ADDR);
}

static int read_register(uint8_t* dst, uint8_t reg_addr) {
  return read_registers(dst, 1, reg_addr);
}

static int write_registers(uint8_t reg_addr, uint8_t* data, size_t n_data) {
  if (n_data > (TX_BUF_SIZE - 1))
    return -1;

  tx_buf[0] = reg_addr;
  memcpy(tx_buf + 1, data, n_data);

  return riotee_i2c_write(AM1805_I2C_ADDR, tx_buf, n_data + 1);
}

static int write_register(uint8_t reg_addr, uint8_t data) {
  tx_buf[0] = reg_addr;
  tx_buf[1] = data;
  return riotee_i2c_write(AM1805_I2C_ADDR, tx_buf, 2);
}

static int set_configuration_key(uint8_t key) {
  return write_register(AM1805_CONFIG_KEY_REG, key);
}

static int get_id(uint16_t* id) {
  int rc;
  if ((rc = read_registers((uint8_t*)id, 2, AM1805_ID0_REG)) != 0)
    return rc;
  return 0;
}

/* Waits until alarm register accepts data. Seems to be undocumented behavior of AM1805 */
static int wait_for_alarm_ok() {
  uint8_t check;
  int retries = N_RETRIES;
  do {
    retries--;
    write_register(AM1805_SECOND_ALARM_REG, 0x55);
    read_register(&check, AM1805_SECOND_ALARM_REG);
    if (check == 0x55)
      return 0;
    riotee_sleep_ms(RETRY_DELAY_MS);
  } while (retries > 0);
  return -1;
}

int am1805_init(void) {
  uint16_t id;
  int retries = N_RETRIES;
  do {
    retries--;
    if (get_id(&id) == 0)
      if (id == 0x0518) {
        return 0;
      }
    riotee_sleep_ms(RETRY_DELAY_MS);
  } while (retries > 0);
  return -1;
}

int am1805_enable_trickle(void) {
  /* Sets lowest thresholds for VBAT */
  set_configuration_key(AM1805_CONFIG_KEY_OTHERS);
  write_register(AM1805_BREF_CONTROL_REG, 0xF0);

  /* Enable trickle charging of capacitors on VBAT */
  set_configuration_key(AM1805_CONFIG_KEY_OTHERS);
  write_register(AM1805_TRICKLE_REG,
                 AM1805_TRICKLE_REG_TCS_MSK | AM1805_TRICKLE_REG_DIODE_SCHOTTKY_MSK | AM1805_TRICKLE_REG_ROUT_6K_MSK);

  return 0;
}

int am1805_get_datetime(struct tm* t) {
  uint8_t rx_buf[7];
  memset(t, 0, sizeof(t));

  read_registers(rx_buf, 7, AM1805_SECOND_REG);

  t->tm_sec = hex2dec(rx_buf[0] & AM1805_SECOND_MSK);
  t->tm_min = hex2dec(rx_buf[1] & AM1805_MINUTE_MSK);
  t->tm_hour = hex2dec(rx_buf[2] & AM1805_HOUR_24_MSK);
  t->tm_mday = hex2dec(rx_buf[3] & AM1805_DATE_MSK);
  // struct tm expect month from 0-11, and RTC save from 1-12
  t->tm_mon = hex2dec(rx_buf[4] & AM1805_MONTH_MSK) - 1;
  // struct tm expect years since 1900, and RTC save from 00~99 which is 2000~2099
  t->tm_year = hex2dec(rx_buf[5] & AM1805_YEAR_MSK) + 100;
  t->tm_wday = hex2dec(rx_buf[6] & AM1805_DAY_MSK);
  return 0;
}

int am1805_set_datetime(struct tm* t) {
  uint8_t time_buf[7];
  time_buf[0] = dec2hex(t->tm_sec);
  time_buf[1] = dec2hex(t->tm_min);
  time_buf[2] = dec2hex(t->tm_hour);
  time_buf[3] = dec2hex(t->tm_mday);
  time_buf[4] = dec2hex(t->tm_mon + 1);
  time_buf[5] = dec2hex(t->tm_year - 100);
  time_buf[6] = dec2hex(t->tm_wday);

  return write_registers(AM1805_SECOND_REG, time_buf, sizeof(time_buf));
}

int am1805_get_hundredths(unsigned int* hundredths) {
  int rc;
  uint8_t rx_buf;
  if ((rc = read_register(&rx_buf, AM1805_HUNDRETH_REG)) != 0)
    return rc;
  *hundredths = (unsigned int)hex2dec(rx_buf);
  return 0;
}

int am1805_get_alarm(struct tm* t) {
  uint8_t rx_buf[6];
  memset(t, 0, sizeof(t));

  read_registers(rx_buf, 6, AM1805_SECOND_ALARM_REG);

  t->tm_sec = hex2dec(rx_buf[0] & AM1805_SECOND_MSK);
  t->tm_min = hex2dec(rx_buf[1] & AM1805_MINUTE_MSK);
  t->tm_hour = hex2dec(rx_buf[2] & AM1805_HOUR_24_MSK);
  t->tm_mday = hex2dec(rx_buf[3] & AM1805_DATE_MSK);
  // struct tm expect month from 0-11, and RTC save from 1-12
  t->tm_mon = hex2dec(rx_buf[4] & AM1805_MONTH_MSK) - 1;
  t->tm_wday = hex2dec(rx_buf[5] & AM1805_DAY_MSK);
  return 0;
}

int am1805_reset(void) {
  set_configuration_key(AM1805_CONFIG_KEY_RESET);
  return 0;
}

int am1805_set_alarm(struct tm* t_alarm) {
  int rc;
  uint8_t time_buf[5];

  /* For some reason, the alarm register does not accept data for a while after reset/POR */
  if ((rc = wait_for_alarm_ok()) != 0)
    return rc;

  time_buf[0] = dec2hex(t_alarm->tm_sec);
  time_buf[1] = dec2hex(t_alarm->tm_min);
  time_buf[2] = dec2hex(t_alarm->tm_hour);
  time_buf[3] = dec2hex(t_alarm->tm_mday);
  time_buf[4] = dec2hex(t_alarm->tm_mon + 1);
  if ((rc = write_registers(AM1805_SECOND_ALARM_REG, time_buf, sizeof(time_buf))) != 0)
    return rc;
  if ((rc = write_register(AM1805_INT_MSK_REG, AM1805_INTERRUPT_AIE_MSK)) != 0)
    return rc;
  if ((rc = write_register(AM1805_TIMER_CONTROL_REG, AM1805_TIMER_CTRL_RPT_DAY_MSK)) != 0)
    return rc;
  return 0;
}

int am1805_get_status(uint8_t* dst) {
  return read_register(dst, AM1805_STATUS_REG);
}

int am1805_disable_power(void) {
  uint8_t readback;
  write_register(AM1805_CONTROL1_REG, AM1805_CONTROL1_PWR2_MSK);
  write_register(AM1805_CONTROL2_REG, AM1805_CONTROL2_OUT2S_SLEEP_MSK);
  write_register(AM1805_SLEEP_CONTROL_REG, 0x80);
  read_register(&readback, AM1805_SLEEP_CONTROL_REG);
  if ((readback & 0x80) == 0) {
    return -1;
  }
  return 0;
}