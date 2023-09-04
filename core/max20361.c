#include "nrf.h"

#include <stdbool.h>
#include <stddef.h>

#include "riotee_i2c.h"
#include "riotee_max20361.h"

#define I2C_DEVICE_ADDRESS (0x15)

#define MAX_DEVCNTL_FRCWAKE_HIGH_Pos (2UL)
#define MAX_DEVCNTL_FRCWAKE_HIGH_Msk (0x1UL << MAX_DEVCNTL_FRCWAKE_HIGH_Pos)

#define MAX_INTMSK_WAKEBST_Pos (0UL)
#define MAX_INTMSK_WAKEBST_Msk (0x1UL << MAX_INTMSK_WAKEBST_Pos)
#define MAX_INTMSK_ENBSTAT_Pos (1UL)
#define MAX_INTMSK_ENBSTAT_Msk (0x1UL << MAX_INTMSK_ENBSTAT_Pos)
#define MAX_INTMSK_VOCVALID_Pos (2UL)
#define MAX_INTMSK_VOCVALID_Msk (0x1UL << MAX_INTMSK_VOCVALID_Pos)
#define MAX_INTMSK_VOCRDY_Pos (3UL)
#define MAX_INTMSK_VOCRDY_Msk (0x1UL << MAX_INTMSK_VOCRDY_Pos)
#define MAX_INTMSK_HARRDY_Pos (4UL)
#define MAX_INTMSK_HARRDY_Msk (0x1UL << MAX_INTMSK_HARRDY_Pos)

#define MAX_SYSREGCFG_SYSREG_Pos (0UL)
#define MAX_SYSREGCFG_SYSRECHG_Pos (4UL)
#define MAX_SYSREGCFG_SYSBATSEL_Pos (7UL)

#define MAX_SYSREGCFG_SYSREG_4000MV (0x0UL)
#define MAX_SYSREGCFG_SYSREG_4050MV (0x1UL)
#define MAX_SYSREGCFG_SYSREG_4100MV (0x2UL)
#define MAX_SYSREGCFG_SYSREG_4150MV (0x3UL)
#define MAX_SYSREGCFG_SYSREG_4200MV (0x4UL)
#define MAX_SYSREGCFG_SYSREG_4250MV (0x5UL)
#define MAX_SYSREGCFG_SYSREG_4300MV (0x6UL)
#define MAX_SYSREGCFG_SYSREG_4350MV (0x7UL)
#define MAX_SYSREGCFG_SYSREG_4400MV (0x8UL)
#define MAX_SYSREGCFG_SYSREG_4450MV (0x9UL)
#define MAX_SYSREGCFG_SYSREG_4500MV (0xAUL)
#define MAX_SYSREGCFG_SYSREG_4550MV (0xBUL)
#define MAX_SYSREGCFG_SYSREG_4600MV (0xCUL)
#define MAX_SYSREGCFG_SYSREG_4650MV (0xDUL)
#define MAX_SYSREGCFG_SYSREG_4700MV (0xEUL)
#define MAX_SYSREGCFG_SYSREG_4750MV (0xFUL)

#define MAX_SYSREGCFG_SYSRECHG_25MV (0x0UL)
#define MAX_SYSREGCFG_SYSRECHG_50MV (0x1UL)
#define MAX_SYSREGCFG_SYSRECHG_75MV (0x2UL)
#define MAX_SYSREGCFG_SYSRECHG_100MV (0x3UL)
#define MAX_SYSREGCFG_SYSRECHG_150MV (0x4UL)
#define MAX_SYSREGCFG_SYSRECHG_200MV (0x5UL)
#define MAX_SYSREGCFG_SYSRECHG_250MV (0x6UL)
#define MAX_SYSREGCFG_SYSRECHG_300MV (0x7UL)

int riotee_max20361_init() {
  int rc;
  uint8_t tx_buf[2];

  /* Disable the FRCWAKE thresholding */
  tx_buf[0] = MAX20361_REG_DEV_CNTL;
  tx_buf[1] = MAX_DEVCNTL_FRCWAKE_HIGH_Msk;
  if ((rc = riotee_i2c_write_atomic(I2C_DEVICE_ADDRESS, tx_buf, 2)) != 0)
    return rc;

  /* Increase the maximum storage voltage to 4.75V */
  tx_buf[0] = MAX20361_REG_SYS_REG_CFG;
  tx_buf[1] = (MAX_SYSREGCFG_SYSREG_4750MV << MAX_SYSREGCFG_SYSREG_Pos);
  return riotee_i2c_write_atomic(I2C_DEVICE_ADDRESS, tx_buf, 2);
}

int riotee_max20361_id(uint8_t *dev_id) {
  int rc;
  uint8_t tx_buf;
  tx_buf = MAX20361_REG_DEV_ID;
  if ((rc = riotee_i2c_write(I2C_DEVICE_ADDRESS, &tx_buf, 1)) != 0)
    return rc;

  return riotee_i2c_read(dev_id, 1, I2C_DEVICE_ADDRESS);
}
