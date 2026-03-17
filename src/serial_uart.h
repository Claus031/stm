#ifndef SERIAL_UART_H
#define SERIAL_UART_H

#include <stddef.h>

void serial_uart_init(void);
void serial_uart_write_byte(char byte);
void serial_uart_write(const char *buffer, size_t length);

#endif
