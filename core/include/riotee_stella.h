/**
 * @defgroup stella Stella protocol
 *  @{
 *
 */
#ifndef __STELLA_H_
#define __STELLA_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIOTEE_STELLA_MAX_DATA (255 - sizeof(riotee_stella_pkt_header_t))

typedef struct __attribute__((packed)) {
  /* ID of the sender of this packet. */
  uint32_t dev_id;
  /* ID of the packet. */
  uint16_t pkt_id;
  /* ID of a previous packet that is acknowledged with this packet. */
  uint16_t ack_id;
} riotee_stella_pkt_header_t;

typedef struct __attribute__((packed)) {
  /* Lenght of the packet, excluding this one byte length field. */
  uint8_t len;
  /* Packet header. */
  riotee_stella_pkt_header_t hdr;
  /* Payload data. */
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
 * @return STELLA_ERR_OK if acknowledgement is received.
 */
int riotee_stella_transceive(riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt);

/* Wrapper function for simple uplink data. Returns STELLA_ERR_OK if acknowledgement is received. */
/**
 * @brief Sends data in a Stella packet.
 *
 * @param data Pointer to data.
 * @param n Size of data.
 *
 * @return STELLA_ERR_OK if acknowledgement is received.
 */
int riotee_stella_send(void *data, size_t n);

/**
 * @brief Sets the ID that is used when sending packets with riotee_stella_send()
 *
 * @param dev_id
 */
void riotee_stella_set_id(uint32_t dev_id);

enum { STELLA_ERR_OK = 0, STELLA_ERR_GENERIC = -1, STELLA_ERR_RESET = -2, STELLA_ERR_NOACK = 1 };

#ifdef __cplusplus
}
#endif

#endif /* __STELLA_H_ */

/** @} */