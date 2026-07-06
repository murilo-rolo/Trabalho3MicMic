#define F_CPU 16000000UL

#include "teclado.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

static const char keymap[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

static volatile char tecla_buffer = 0;
static char tecla_anterior = 0;

static char scan_matrix(void)
{
	for (uint8_t i = 0; i < 4; i++) {
		PORTB |= 0x0F;
		PORTB &= ~(0x08 >> i);
		_delay_us(50);
		for (uint8_t j = 0; j < 4; j++) {
			if ((PIND & (0x80 >> j)) == 0) {
				PORTB |= 0x0F;
				return keymap[i][j];
			}
		}
	}
	PORTB |= 0x0F;
	return 0;
}

ISR(TIMER2_COMPA_vect)
{
	char t = scan_matrix();
	if (t != 0 && t != tecla_anterior) {
		tecla_buffer = t;
	}
	tecla_anterior = t;
}

void setup_teclado(void)
{
	DDRB |= 0x0F;
	PORTB |= 0x0F;

	DDRD &= ~0xF0;
	PORTD |= 0xF0;

	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	OCR2A = 156;
	TIMSK2 = (1 << OCIE2A);
}

char teclado_get_key(void)
{
	char t = tecla_buffer;
	if (t) {
		tecla_buffer = 0;
		printf("%c", t);
	}
	return t;
}
