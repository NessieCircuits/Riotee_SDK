#ifndef __STELLA_H_
#define __STELLA_H_

#include <stdint.h>
#include <stddef.h>

typedef struct __attribute__((packed)) {
  /* ID of the sender of this packet */
  uint32_t dev_id;
  /* ID of the packet */
  uint16_t pkt_id;
  /* ID of a previous packet that is acknowledged with this packet */
  uint16_t ack_id;
} riotee_stella_pkt_header_t;

typedef struct __attribute__((packed)) {
  /* Lenght of the packet, excluding this one byte length field */
  uint8_t len;
  riotee_stella_pkt_header_t hdr;
  /* Max payload size is 255. We have 8 Byte protocol overhead. */
  uint8_t data[255 - sizeof(riotee_stella_pkt_header_t)];
} riotee_stella_pkt_t;

int riotee_stella_init(void);

/* Transmits tx_pkt and receives downlink packet into rx_pkt. Returns STELLA_ERR_OK if acknowledgement is received. */
int riotee_stella_transceive(riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt);

/* Wrapper function for simple uplink data. Returns STELLA_ERR_OK if acknowledgement is received. */
int riotee_stella_send(void *data, size_t n);

/* Set the ID that our device uses to identify to the nework */
void riotee_stella_set_id(uint32_t dev_id);

enum { STELLA_ERR_OK = 0, STELLA_ERR_GENERIC = -1, STELLA_ERR_RESET = -2, STELLA_ERR_NOACK = 1 };

#endif /* __STELLA_H_ */