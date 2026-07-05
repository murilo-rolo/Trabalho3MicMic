#define F_CPU 16000000UL

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t ms = 0;

ISR(TIMER0_COMPA_vect)
{
	ms++;
}

void timer_init(void)
{
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 249;
	TIMSK0 = (1 << OCIE0A);
}

uint32_t millis(void)
{
	uint32_t tmp;
	cli();
	tmp = ms;
	sei();
	return tmp;
}
