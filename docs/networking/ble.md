# Bluetooth Low Energy

Riotee has limited support for BLE. You can use the API to send advertisement packets with a custom payload on the three advertising channels.

The implementation is not standard-conformant. You may only use it for evaluation purposes in dedicated facilities where it cannot interfere with other spectrum users.


# Reference
::: doxy.riotee.Function
  name: void riotee_ble_init(void)

::: doxy.riotee.Function
  name: int riotee_ble_advertise(void *data, riotee_adv_ch_t ch)

# Example

::: doxy.riotee.Code
  file: main.c
  start: 0
  end: 35