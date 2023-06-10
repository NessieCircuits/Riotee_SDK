#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int uart_init(uint32_t pseltxd, uint32_t baudrate);
void _putchar(char character);
int uart_set_baudrate(uint32_t baudrate);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H_ */