#ifndef __RADIO_H__
#define __RADIO_H__

typedef void (*RADIO_CALLBACK)(void);

typedef enum {
  /* Radio has ramped down */
  RADIO_EVT_DISABLED,
  /* Radio has ramped up for TX */
  RADIO_EVT_TXREADY,
  /* Radio has ramped up for RX */
  RADIO_EVT_RXREADY,
  /* A valid packet has been received */
  RADIO_EVT_CRCOK,
  /* Packet received, but CRC check failed */
  RADIO_EVT_CRCERR,
  /* An address has been decoded */
  RADIO_EVT_ADDRESS
} radio_evt_t;

/**
 * @brief Registers a callback in the radio isr.
 *
 * @param evt Event for which callback is registered
 * @param cb Pointer to callback function
 * @return int
 */
int radio_cb_register(radio_evt_t evt, RADIO_CALLBACK cb);

/**
 * @brief Unregisters a previously registered callback.
 *
 * @param evt Event for which callback was registered.
 * @return int
 */
int radio_cb_unregister(radio_evt_t evt);

/**
 * @brief Initializes the radio peripheral. Must be called before radio can be used.
 *
 */
void radio_init();

/**
 * @brief Starts the radio by starting the HFXO, which automatically starts the radio when its up.
 *
 */
void radio_start();

/**
 * @brief Stops the radio and disables the HFXO.
 *
 */
void radio_stop();

#endif /* __RADIO_H__ */