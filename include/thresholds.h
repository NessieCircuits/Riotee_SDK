#ifndef __THRESHOLDS_H_
#define __THRESHOLDS_H_

typedef enum {
  THR_LOW_2V16 = 0,
  THR_LOW_2V39 = 3,
  THR_LOW_2V56 = 1,
  THR_LOW_2V71 = 6,
  THR_LOW_2V89 = 4,
  THR_LOW_3V00 = 2,
  THR_LOW_3V22 = 7,
  THR_LOW_3V42 = 5,
  THR_LOW_3V64 = 8
} thr_low_t;

typedef enum {
  THR_HIGH_2V81 = 0,
  THR_HIGH_3V01 = 1,
  THR_HIGH_3V24 = 3,
  THR_HIGH_3V47 = 2,
  THR_HIGH_3V70 = 4,
  THR_HIGH_3V94 = 6,
  THR_HIGH_4V19 = 5,
  THR_HIGH_4V38 = 7,
  THR_HIGH_4V61 = 8
} thr_high_t;

/* The lowest power setting is when both inputs are High-Z */
#define THR_LOW_POWER 4

#if defined __cplusplus
extern "C" {
#endif

int thresholds_low_set(thr_low_t thr);
int thresholds_high_set(thr_high_t thr);
#if defined __cplusplus
}
#endif

#endif /* __THRESHOLDS_H_ */