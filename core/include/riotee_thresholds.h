#ifndef __THRESHOLDS_H_
#define __THRESHOLDS_H_

typedef enum { THR_LOW_2V5 = 0, THR_LOW_3V1 = 6, THR_LOW_3V5 = 2, THR_LOW_4V1 = 8 } thr_low_t;

typedef enum {
  THR_HIGH_3V0 = 0,
  THR_HIGH_3V2 = 3,
  THR_HIGH_3V4 = 1,
  THR_HIGH_3V6 = 6,
  THR_HIGH_3V8 = 4,
  THR_HIGH_4V0 = 2,
  THR_HIGH_4V2 = 7,
  THR_HIGH_4V4 = 5,
  THR_HIGH_4V6 = 8
} thr_high_t;

/* The lowest power setting is when both inputs are High-Z */
#define THR_LOW_POWER 4

#if defined __cplusplus
extern "C" {
#endif

/**
 * @brief Sets the 'low' threshold that triggers suspension of the user task and checkpoining.
 *
 * Changing of the threshold takes multiple milliseconds to take effect.
 *
 * @param thr Selected threshold.
 *
 */
void riotee_thresholds_low_set(thr_low_t thr);

/**
 * @brief Sets the 'high' threshold upon which user task execution is resumed.
 *
 * Changing of the threshold takes multiple milliseconds to take effect.
 *
 * @param thr Selected threshold.
 *
 */
void riotee_thresholds_high_set(thr_high_t thr);
#if defined __cplusplus
}
#endif

#endif /* __THRESHOLDS_H_ */