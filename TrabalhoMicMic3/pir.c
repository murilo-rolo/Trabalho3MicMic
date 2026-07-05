#include "pir.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t pir_flag = 0;

ISR(INT0_vect)
{
	pir_flag = 1;
}

void PIR_Init(void)
{
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);

	EICRA |= (1 << ISC01) | (1 << ISC00);
	EIMSK |= (1 << INT0);
}

uint8_t PIR_Checar(void)
{
	uint8_t ret;
	cli();
	ret = pir_flag;
	pir_flag = 0;
	sei();
	return ret;
}
