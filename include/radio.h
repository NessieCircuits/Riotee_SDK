#ifndef __RADIO_H__
#define __RADIO_H__

typedef void (*RADIO_CALLBACK)(void);

typedef enum {
  RADIO_EVT_DISABLED,
  RADIO_EVT_READY,
  RADIO_EVT_CRCOK
} radio_evt_t;

int radio_cb_register(radio_evt_t evt, RADIO_CALLBACK cb);
int radio_cb_unregister(radio_evt_t evt);
int radio_init();
int radio_start();

#endif /* __RADIO_H__ */