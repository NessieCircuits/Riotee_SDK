#ifndef __RioteeBLE_H_
#define __RioteeBLE_H_

#include <cstddef>

class RioteeBLE {

public:
  void begin(const char adv_name[], size_t data_len);
  void advertise(void *data);
};

extern RioteeBLE BLE;

#endif /* __RioteeBLE_H_ */