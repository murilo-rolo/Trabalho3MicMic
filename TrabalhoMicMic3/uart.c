#define F_CPU 16000000UL

#include "uart.h"
#include <avr/io.h>

void UART_Init(uint32_t baud)
{
	uint16_t ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)(ubrr);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	fdevopen(UART_PutChar, UART_GetChar);
}

int UART_PutChar(char c, FILE *stream)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;
}

int UART_GetChar(FILE *stream)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}
