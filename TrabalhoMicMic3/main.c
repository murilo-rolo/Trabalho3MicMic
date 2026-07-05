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

static uint8_t coletar_digitos(char *buffer, uint8_t max, char primeira)
{
	uint8_t count = 0;
	char tecla;

	if (primeira >= '0' && primeira <= '9') {
		buffer[count++] = primeira;
		LCD_Ponteiro(1, 0);
		LCD_Write_Char('*');
	}

	while (1) {
		tecla = teclado_scan();
		if (tecla == 0) continue;
		if (tecla >= '0' && tecla <= '9') {
			if (count < max) {
				buffer[count] = tecla;
				LCD_Ponteiro(1, count);
				LCD_Write_Char('*');
				count++;
			}
		} else if (tecla == 'D' && count > 0) {
			count--;
			LCD_Ponteiro(1, count);
			LCD_Write_Char(' ');
		} else if (tecla == 'C') {
			return count;
		}
	}
}

int main(void)
{
	I2C_Init();
	LCD_Init();
	setup_teclado();
	PIR_Init();

	DDRD |= (1 << PD3);
	DDRB |= (1 << PB4) | (1 << PB5);

	if (!senha_existe()) {
		setar_senha("123456");
		eeprom_write_byte((uint8_t*)ENDERECO_FLAG, MAGIC_FLAG);
	}

	sei();

	estado = DESARMADO;
	LCD_Clear();
	LCD_Write_String("Desarmado");
	led_des_on();
	led_arm_off();
	buzz_off();

	while (1) {
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
				char digito_buf[TAM];

				tecla = teclado_scan();
				if (tecla != 0) {
					ultimas[0] = ultimas[1];
					ultimas[1] = ultimas[2];
					ultimas[2] = tecla;

					if (tecla >= '0' && tecla <= '9') {
						uint8_t n = coletar_digitos(digito_buf, TAM, tecla);
						if (n == TAM) {
							ler_senha(senha_eeprom);
							if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
								estado = ARMADO;
							} else {
								LCD_Clear();
								LCD_Write_String("Senha Incorreta!");
								_delay_ms(1500);
							}
						} else {
							LCD_Clear();
							LCD_Write_String("Desarmado");
						}
					} else if (ultimas[0] == 'A' && ultimas[1] == 'B' && ultimas[2] == 'A') {
						estado = CONFIG;
					}
				}
				break;
			}

			case ARMADO:
			{
				char digito_buf[TAM];

				if (PIR_Checar()) {
					estado = ALARME;
					break;
				}

				tecla = teclado_scan();
				if (tecla >= '0' && tecla <= '9') {
					uint8_t n = coletar_digitos(digito_buf, TAM, tecla);
					if (n == TAM) {
						ler_senha(senha_eeprom);
						if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
							estado = DESARMADO;
						} else {
							LCD_Clear();
							LCD_Write_String("Senha Incorreta!");
							_delay_ms(1500);
							ult_estado = 0xFF;
						}
					} else {
						ult_estado = 0xFF;
					}
				}
				break;
			}

			case ALARME:
			{
				static uint16_t pisca = 0;
				char digito_buf[TAM];

				pisca++;
				if (pisca >= 200) {
					pisca = 0;
					PORTB ^= (1 << PB4) | (1 << PB5);
				}

				tecla = teclado_scan();
				if (tecla >= '0' && tecla <= '9') {
					uint8_t n = coletar_digitos(digito_buf, TAM, tecla);
					if (n == TAM) {
						ler_senha(senha_eeprom);
						if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
							estado = DESARMADO;
						} else {
							LCD_Clear();
							LCD_Write_String("Senha Incorreta!");
							_delay_ms(1500);
							ult_estado = 0xFF;
						}
					} else {
						ult_estado = 0xFF;
					}
				}
				break;
			}

			case CONFIG:
			{
				char senha1[TAM], senha2[TAM];

				LCD_Clear();
				LCD_Write_String("Nova senha (6):");
				if (coletar_digitos(senha1, TAM, 0) != TAM) {
					estado = DESARMADO;
					break;
				}

				LCD_Clear();
				LCD_Write_String("Confirme:");
				if (coletar_digitos(senha2, TAM, 0) != TAM) {
					estado = DESARMADO;
					break;
				}

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
}
