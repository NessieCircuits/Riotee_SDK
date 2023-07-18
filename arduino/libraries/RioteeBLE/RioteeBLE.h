#ifndef __RioteeBLE_H_
#define __RioteeBLE_H_

#include <cstddef>

class RioteeBLE {
 public:
  void begin(const char adv_name[], void *data, size_t data_len);
  void advertise();
};

extern RioteeBLE BLE;

#endif /* __RioteeBLE_H_ */