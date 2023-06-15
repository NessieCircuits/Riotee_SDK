#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int riotee_uart_init(uint32_t pseltxd, uint32_t baudrate);
int riotee_uart_set_baudrate(uint32_t baudrate);

void _putchar(char character);
#define riotee_putc(x) _putchar(x)

#ifdef __cplusplus
}
#endif

#endif /* __UART_H_ */