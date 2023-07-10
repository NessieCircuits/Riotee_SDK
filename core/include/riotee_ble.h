#ifndef __RIOTEE_BLE_H__
#define __RIOTEE_BLE_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of advertisement
 *
 */
typedef enum {
  /** Undirected, connectable advertisement. */
  ADV_IND = 0,
  /** Directed, connectable advertisement. */
  ADV_DIRECT_IND = 1,
  /** Undirected, unconnectable advertisement. */
  ADV_NONCONN_IND = 2,
  /** Scan request. */
  SCAN_REQ = 3,
  /** Scan response. */
  SCAN_RSP = 4,
  /** Connection request. */
  CONNECT_REQ = 5,
  /** Scannable undirected advertisement. */
  ADV_SCAN_IND = 6,
} riotee_adv_pdu_type_t;

typedef enum { ADV_CH_37 = 37, ADV_CH_38 = 38, ADV_CH_39 = 39, ADV_CH_ALL = 255 } riotee_adv_ch_t;

typedef struct {
  uint8_t addr_bytes[6];
} __attribute__((__packed__)) riotee_ble_ll_addr_t;

typedef struct {
  riotee_adv_pdu_type_t pdu_type : 4;
  unsigned int rfu : 1;
  unsigned int chsel : 1;
  unsigned int txadd : 1;
  unsigned int rxadd : 1;
  uint8_t len;
} __attribute__((__packed__)) riotee_ble_ll_header_t;

/** Common packet format for some ADV PDU types */
typedef struct {
  riotee_ble_ll_header_t header;
  riotee_ble_ll_addr_t adv_addr;
  uint8_t payload[31];
} __attribute__((__packed__)) riotee_adv_pck_t;

/**
 * @brief Sets up the internal packet buffer for advertisting with given name, address and payload size.
 *
 * @param adv_addr Pointer to adress buffer.
 * @param adv_name Advertising name of the device.
 * @param name_len Length of the advertising name.
 * @param data_len Size of the payload.
 * @return int Size of remaining unused payload (>=0)
 */
int riotee_ble_prepare_adv(riotee_ble_ll_addr_t *adv_addr, const char adv_name[], size_t name_len, size_t data_len);

/**
 * @brief Advertises the given payload on the selected channel(s)
 *
 * @param data Pointer to payload.
 * @param ch Channel(s) on which advertisement should be sent.
 * @return int 0 on success
 */
int riotee_ble_advertise(void *data, riotee_adv_ch_t ch);

/**
 * @brief Initializes BLE driver.
 *
 */
void riotee_ble_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_H__ */