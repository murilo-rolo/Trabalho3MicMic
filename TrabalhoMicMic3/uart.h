#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdio.h>

void UART_Init(uint32_t baud);
int UART_PutChar(char c, FILE *stream);
int UART_GetChar(FILE *stream);

#endif
