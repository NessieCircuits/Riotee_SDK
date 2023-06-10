#ifndef __BLE_H__
#define __BLE_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
  ADV_IND = 0,
  ADV_DIRECT_IND = 1,
  ADV_NONCONN_IND = 2,
  SCAN_REQ = 3,
  SCAN_RSP = 4,
  CONNECT_REQ = 5,
  ADV_SCAN_IND = 6,
} adv_pdu_type_t;

typedef enum { ADV_CH_37 = 37, ADV_CH_38 = 38, ADV_CH_39 = 39, ADV_CH_ALL = 255 } adv_ch_t;

typedef struct {
  uint8_t addr_bytes[6];
} __attribute__((__packed__)) ble_ll_addr_t;

typedef struct {
  adv_pdu_type_t pdu_type : 4;
  unsigned int rfu : 1;
  unsigned int chsel : 1;
  unsigned int txadd : 1;
  unsigned int rxadd : 1;
  uint8_t len;
} __attribute__((__packed__)) ble_ll_header_t;

// This format is common to a number of ADV PDU types
typedef struct {
  ble_ll_header_t header;
  ble_ll_addr_t adv_addr;
  uint8_t payload[31];
} __attribute__((__packed__)) adv_pck_t;

/* Setup the internal packet buffer for advertisting with given name, address and payload size */
int ble_prepare_adv(ble_ll_addr_t *adv_addr, const char adv_name[], size_t name_len, size_t data_len);

/* Advertise the given payload on the selected channel(s) */
int ble_advertise(void *data, adv_ch_t ch);

int ble_init();

#ifdef __cplusplus
}
#endif

#endif /* __BLE_H__ */