#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "teclado.h"

static const char keymap[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

static char tecla_anterior = 0;

void setup_teclado(void)
{
	DDRD |= 0xF0;
	PORTD |= 0xF0;

	DDRB &= ~0x0F;
	PORTB |= 0x0F;
}

char teclado_scan(void)
{
	char tecla = 0;
	uint8_t achou = 0;

	for (uint8_t i = 0; i < 4 && !achou; i += 1) {
		PORTD |= 0xF0;
		PORTD &= ~(1 << (4 + i));
		_delay_us(50);

		for (uint8_t j = 0; j < 4; j += 1) {
			if ((PINB & (1 << j)) == 0) {
				tecla = keymap[i][j];
				achou = 1;
				break;
			}
		}
	}

	PORTD |= 0xF0;

	if (tecla != 0) {
		if (tecla != tecla_anterior) {
			_delay_ms(20);
			tecla_anterior = tecla;
			return tecla;
		}
		return 0;
	}

	tecla_anterior = 0;
	return 0;
}