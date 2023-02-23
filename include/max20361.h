#ifndef __MAX20361_H_
#define __MAX20361_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum max20361_reg_map {
  REG_DEV_ID,
  REG_STATUS,
  REG_INT,
  REG_INT_MSK,
  REG_SYS_REG_CFG,
  REG_WAKE_CFG,
  REG_MPPT_CFG,
  REG_MEAS_CFG,
  REG_DEV_CNTL,
  REG_VOC_MEAS,
  REG_HARV_CNT_H,
  REG_HARV_CNT_L,
  REG_SLEEP_THD,
};

enum max20361_wake_thr {
  THR_3V0,
  THR_3V1,
  THR_3V2,
  THR_3V3,
  THR_3V4,
  THR_3V5,
  THR_3V6,
  THR_3V7,
};

enum max20361_t_per {
  T_MEAS_x64,
  T_MEAS_x128,
  T_MEAS_x256,
  DISABLED,
};

int max20361_init(void);
int max20361_id(uint8_t *dev_id);

#ifdef __cplusplus
}
#endif

#endif /* __MAX20361_H_ */