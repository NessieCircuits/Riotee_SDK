#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int uart_init(uint32_t pseltxd);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H_ */