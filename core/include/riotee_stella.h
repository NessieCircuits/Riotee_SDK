/**
 * @defgroup stella Stella protocol
 *  @{
 */
#ifndef __RIOTEE_STELLA_H_
#define __RIOTEE_STELLA_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum size of payload in stella packet. */
#define RIOTEE_STELLA_MAX_DATA (255 - sizeof(riotee_stella_pkt_header_t))

typedef struct __attribute__((packed)) {
  /** ID of the device sending/receiving this packet. */
  uint32_t dev_id;
  /** ID of the packet. */
  uint16_t pkt_id;
  /** ID of a previous packet that is acknowledged with this packet. */
  uint16_t ack_id;
} riotee_stella_pkt_header_t;

typedef struct __attribute__((packed)) {
  /** Length of the packet, excluding this one byte length field. */
  uint8_t len;
  /** Packet header. */
  riotee_stella_pkt_header_t hdr;
  /** Payload data. */
  uint8_t data[RIOTEE_STELLA_MAX_DATA];
} riotee_stella_pkt_t;

/**
 * @brief Initializes radio and protocol. Must be called once after reset before using the module.
 */
void riotee_stella_init(void);

/**
 * @brief Transmits uplink tx_pkt and receives downlink packet into rx_pkt.
 *
 * @param rx_pkt Pointer to buffer where received packet is stored.
 * @param tx_pkt Pointer to packet that is sent.
 *
 * @retval RIOTEE_SUCCESS                 Packet successfully sent and acknowledged.
 * @retval RIOTEE_ERR_RESET              Reset occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_TEARDOWN           Teardown occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_STELLA_NOACK       Packet sent, but no acknowledgement received.
 * @retval RIOTEE_ERR_STELLA_INVALIDACK  Invalid acknowledgement received.
 */
riotee_rc_t riotee_stella_transceive(riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt);

/**
 * @brief Sends data in a Stella packet.
 *
 * @param data Pointer to data.
 * @param n Size of data.
 *
 * @retval RIOTEE_SUCCESS                 Packet successfully sent and acknowledged.
 * @retval RIOTEE_ERR_RESET              Reset occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_TEARDOWN           Teardown occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_STELLA_NOACK       Packet sent, but no acknowledgement received.
 * @retval RIOTEE_ERR_STELLA_INVALIDACK  Invalid acknowledgement received.
 */
riotee_rc_t riotee_stella_send(void *data, size_t n);

/**
 * @brief Sets the ID that is used when sending packets with riotee_stella_send()
 *
 * @param dev_id
 */
void riotee_stella_set_id(uint32_t dev_id);

enum {
  /** No acknowledgment received. */
  RIOTEE_ERR_STELLA_NOACK = -(RIOTEE_RC_STELLA_BASE + 1),
  RIOTEE_ERR_STELLA_INVALIDACK = -(RIOTEE_RC_STELLA_BASE + 2)
};

#ifdef __cplusplus
}
#endif

#endif /** @} __RIOTEE_STELLA_H_ */
