/**
 * @defgroup stella Stella protocol
 *  @{
 */
#ifndef __RIOTEE_STELLA_H_
#define __RIOTEE_STELLA_H_

#include <stdint.h>
#include <stddef.h>

#include "riotee.h"

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
 * @brief Sends data to the basestation in a Stella packet.
 *
 * Sends a packet with the specified data as payload to the basestation. IMPORTANT: If the basestation sends data in the
 * acknowledgement packet, this data is discarded and the function returns RIOTEE_ERR_OVERFLOW.
 *
 * @param tx_data Pointer to data.
 * @param tx_size Size of data.
 *
 * @retval RIOTEE_SUCCESS                Packet successfully sent and acknowledged.
 * @retval RIOTEE_ERR_OVERFLOW           tx_size exceeds maximum packet size or basestation sent a payload.
 * @retval RIOTEE_ERR_RESET              Reset occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_TEARDOWN           Teardown occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_STELLA_NOACK       Packet sent, but no acknowledgement received.
 * @retval RIOTEE_ERR_STELLA_INVALIDACK  Invalid acknowledgement received.
 */
riotee_rc_t riotee_stella_send(void *tx_data, size_t tx_size);

/**
 * @brief Receives a packet from the basestation.
 *
 * Sends an empty packet to the basestation and receives the response packet in the provided destination buffer. The
 * destination buffer should have the size of the payload if known or RIOTEE_STELLA_MAX_DATA otherwise. Returns the
 * number of bytes copied into the destination buffer or an error code.
 *
 * @param rx_buf Pointer to destination buffer.
 * @param rx_size Size of destination buffer.
 *
 * @returns Size of received payload in bytes or error code.
 *
 * @retval RIOTEE_SUCCESS                 Packet successfully sent and acknowledged.
 * @retval RIOTEE_ERR_OVERFLOW            Received payload is bigger than rx_size.
 * @retval RIOTEE_ERR_RESET               Reset occured while sending/receiving response.
 * @retval RIOTEE_ERR_TEARDOWN            Teardown occured while sending/receiving response.
 * @retval RIOTEE_ERR_STELLA_NOACK        Packet sent, but no response received.
 * @retval RIOTEE_ERR_STELLA_INVALIDACK   Invalid response received.
 */
riotee_rc_t riotee_stella_receive(uint8_t *rx_buf, size_t rx_size);

/**
 * @brief Sends data in a Stella packet and receives response from basestation.
 *
 * @param rx_buf  Pointer to destination buffer.
 * @param rx_size Size of destination buffer.
 * @param tx_data Pointer to TX data.
 * @param tx_size Size of TX data.
 *
 * @returns Size of received payload in bytes or error code.
 *
 * @retval RIOTEE_SUCCESS                Packet successfully sent and acknowledged.
 * @retval RIOTEE_ERR_OVERFLOW           tx_size exceeds maximum payload or received payload is bigger than rx_size
 * @retval RIOTEE_ERR_RESET              Reset occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_TEARDOWN           Teardown occured while sending/receiving acknowledgement.
 * @retval RIOTEE_ERR_STELLA_NOACK       Packet sent, but no acknowledgement received.
 * @retval RIOTEE_ERR_STELLA_INVALIDACK  Invalid acknowledgement received.
 */
riotee_rc_t riotee_stella_transceive(uint8_t *rx_buf, size_t rx_size, void *tx_data, size_t tx_size);

/**
 * @brief Sets the ID that is used when sending packets with riotee_stella_send()
 *
 * @param dev_id
 */
void riotee_stella_set_id(uint32_t dev_id);

/**
 * @brief Reads the ID that is used when sending packets with riotee_stella_send()
 *
 * @return uint32_t Device ID
 *
 */
uint32_t riotee_stella_get_id(void);

/**
 * @brief Reads internal counter counting number of sent packets.
 *
 * @return unsigned int Number of sent packets.
 *
 */
unsigned int riotee_stella_get_packet_counter(void);

/** Stella-specific return codes. */
enum {
  /** No acknowledgement received. */
  RIOTEE_ERR_STELLA_NOACK = -(RIOTEE_RC_STELLA_BASE + 1),
  /** Invalid acknowledgement received*/
  RIOTEE_ERR_STELLA_INVALIDACK = -(RIOTEE_RC_STELLA_BASE + 2)
};

#ifdef __cplusplus
}
#endif

#endif /** @} __RIOTEE_STELLA_H_ */
