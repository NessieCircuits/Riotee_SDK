#ifndef __STELLA_H_
#define __STELLA_H_

#include <stdint.h>

typedef struct __attribute__((packed)) {
  /* ID of the sender of this packet */
  uint32_t dev_id;
  /* ID of the packet */
  uint16_t pkt_id;
  /* ID of a previous packet that is acknowledged with this packet */
  uint16_t ack_id;
} stella_pkt_header_t;

typedef struct __attribute__((packed)) {
  /* Lenght of the packet, excluding this one byte length field */
  uint8_t len;
  stella_pkt_header_t hdr;
  /* Max payload size is 255. We have 8 Byte protocol overhead. */
  uint8_t data[255 - sizeof(stella_pkt_header_t)];
} stella_pkt_t;

int stella_init(void);

/* Transmits tx_pkt and receives downlink packet into rx_pkt. Returns STELLA_ERR_OK if acknowledgement is received. */
int stella_transceive(stella_pkt_t *rx_pkt, stella_pkt_t *tx_pkt);

/* Wrapper function for simple uplink data. Returns STELLA_ERR_OK if acknowledgement is received. */
int stella_send(uint8_t *data, size_t n);

enum { STELLA_ERR_OK = 0, STELLA_ERR_GENERIC = -1, STELLA_ERR_RESET = -2, STELLA_ERR_NOACK = 1 };

#endif /* __STELLA_H_ */