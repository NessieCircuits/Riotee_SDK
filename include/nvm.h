#ifndef __NVM_H_
#define __NVM_H_

#include <stddef.h>
#include <stdint.h>

#include "nrf.h"

#include "FreeRTOS.h"
#include "semphr.h"

enum { NVM_WRITE = 0x800000, NVM_READ = 0x0 };
typedef uint32_t nvm_transfer_type_t;

int nvm_init();
int nvm_start(nvm_transfer_type_t transfer_type, uint32_t address);
int nvm_write(uint8_t *src, size_t size);
int nvm_read(uint8_t *dst, size_t size);
int nvm_stop(void);

#endif /* __NVM_H_ */
