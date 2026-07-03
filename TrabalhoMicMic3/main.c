#define F_CPU 16000000UL

#include "lcd.h"
#include "teclado.h"
#include "eeprom.h"
#include "pir.h"
#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define TAM 6

typedef enum { DESARMADO, ARMADO, ALARME, CONFIG } EstadoAlarme;

static EstadoAlarme estado = DESARMADO;

static inline void buzz_on(void)  { PORTD |= (1 << PD3); }
static inline void buzz_off(void) { PORTD &= ~(1 << PD3); }
static inline void led_arm_on(void)  { PORTB |= (1 << PB4); }
static inline void led_arm_off(void) { PORTB &= ~(1 << PB4); }
static inline void led_des_on(void)  { PORTB |= (1 << PB5); }
static inline void led_des_off(void) { PORTB &= ~(1 << PB5); }

static void coletar_6_digitos(char *buffer)
{
	uint8_t count = 0;
	char tecla;

	while (count < TAM) {
		tecla = teclado_scan();
		if (tecla >= '0' && tecla <= '9') {
			buffer[count] = tecla;
			LCD_Ponteiro(1, count);
			LCD_Write_Char('*');
			count++;
		}
	}
}

static void sistema_init(void)
{
	I2C_Init();
	LCD_Init();
	setup_teclado();
	PIR_Init();

	DDRD |= (1 << PD3);
	DDRB |= (1 << PB4) | (1 << PB5);

	if (!senha_existe()) {
		setar_senha("123456");
	}

	sei();

	estado = DESARMADO;
	LCD_Clear();
	LCD_Write_String("Desarmado");
	led_des_on();
	led_arm_off();
	buzz_off();
}

static void sistema_loop(void)
{
	static EstadoAlarme ult_estado = 0xFF;
	char tecla;
	char senha_eeprom[TAM];

	if (estado != ult_estado) {
		ult_estado = estado;
		switch (estado) {
			case DESARMADO:
				LCD_Clear();
				LCD_Write_String("Desarmado");
				led_des_on();
				led_arm_off();
				buzz_off();
				break;
			case ARMADO:
				LCD_Clear();
				LCD_Write_String("Armado");
				led_arm_on();
				led_des_off();
				buzz_off();
				break;
			case ALARME:
				LCD_Clear();
				LCD_Write_String("    ALARME!");
				led_arm_on();
				led_des_off();
				buzz_on();
				break;
			case CONFIG:
				break;
		}
	}

	switch (estado) {
		case DESARMADO:
		{
			static char ultimas[3] = {0, 0, 0};
			static char digito_buf[TAM];
			static uint8_t idx = 0;

			tecla = teclado_scan();
			if (tecla != 0) {
				ultimas[0] = ultimas[1];
				ultimas[1] = ultimas[2];
				ultimas[2] = tecla;

				if (tecla >= '0' && tecla <= '9') {
					digito_buf[idx++] = tecla;
					LCD_Ponteiro(1, idx - 1);
					LCD_Write_Char('*');
					if (idx == TAM) {
						ler_senha(senha_eeprom);
						if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
							estado = ARMADO;
						} else {
							LCD_Clear();
							LCD_Write_String("Senha Incorreta!");
							_delay_ms(1500);
						}
						idx = 0;
					}
				} else {
					if (idx > 0) {
						idx = 0;
						LCD_Clear();
						LCD_Write_String("Desarmado");
					} else if (ultimas[0] == 'A' && ultimas[1] == 'B' && ultimas[2] == 'A') {
						estado = CONFIG;
					}
				}
			}
			break;
		}

		case ARMADO:
		{
			static char digito_buf[TAM];
			static uint8_t idx = 0;

			if (PIR_Checar()) {
				idx = 0;
				estado = ALARME;
				break;
			}

			tecla = teclado_scan();
			if (tecla >= '0' && tecla <= '9') {
				digito_buf[idx++] = tecla;
				LCD_Ponteiro(1, idx - 1);
				LCD_Write_Char('*');
				if (idx == TAM) {
					ler_senha(senha_eeprom);
					if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
						idx = 0;
						estado = DESARMADO;
					} else {
						idx = 0;
						LCD_Clear();
						LCD_Write_String("Senha Incorreta!");
						_delay_ms(1500);
						ult_estado = 0xFF;
					}
				}
			} else if (tecla != 0) {
				idx = 0;
			}
			break;
		}

		case ALARME:
		{
			static char digito_buf[TAM];
			static uint8_t idx = 0;
			static uint16_t pisca = 0;

			pisca++;
			if (pisca >= 200) {
				pisca = 0;
				PORTB ^= (1 << PB4) | (1 << PB5);
			}

			tecla = teclado_scan();
			if (tecla >= '0' && tecla <= '9') {
				digito_buf[idx++] = tecla;
				LCD_Ponteiro(1, idx - 1);
				LCD_Write_Char('*');
				if (idx == TAM) {
					ler_senha(senha_eeprom);
					if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
						idx = 0;
						estado = DESARMADO;
					} else {
						idx = 0;
						LCD_Clear();
						LCD_Write_String("Senha Incorreta!");
						_delay_ms(1500);
						ult_estado = 0xFF;
					}
				}
			} else if (tecla != 0) {
				idx = 0;
			}
			break;
		}

		case CONFIG:
		{
			char senha1[TAM], senha2[TAM];

			LCD_Clear();
			LCD_Write_String("Nova senha (6):");
			coletar_6_digitos(senha1);

			LCD_Clear();
			LCD_Write_String("Confirme:");
			coletar_6_digitos(senha2);

			if (memcmp(senha1, senha2, TAM) == 0) {
				setar_senha(senha1);
				LCD_Clear();
				LCD_Write_String("Senha alterada!");
				_delay_ms(1500);
			} else {
				LCD_Clear();
				LCD_Write_String("Erro!");
				_delay_ms(1500);
			}

			estado = DESARMADO;
			break;
		}
	}
}

int main(void)
{
	sistema_init();
	while (1) {
		sistema_loop();
	}
}
