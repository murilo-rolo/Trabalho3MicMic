#define F_CPU 16000000UL

#include "lcd.h"
#include "teclado.h"
#include "eeprom.h"
#include "pir.h"
#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define TAM 6
#define TEMPO_ALARME 10000

typedef enum { DESARMADO, ARMADO, CONFIG } EstadoAlarme;

static EstadoAlarme estado = DESARMADO;
static uint8_t alarme_ativo = 0;
static uint32_t alarme_inicio = 0;
static uint32_t ultimo_blink = 0;

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
		tecla = teclado_get_key();
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
	UART_Init(9600);
	timer_init();
	setup_teclado();
	PIR_Init();

	DDRD |= (1 << PD3);
	DDRB |= (1 << PB4) | (1 << PB5);

	if (!senha_existe()) {
		setar_senha("123456");
		eeprom_write_byte((uint8_t*)ENDERECO_FLAG, MAGIC_FLAG);
	}

	sei();

	printf("Sistema de Alarme Iniciado\n");

	estado = DESARMADO;
	LCD_Clear();
	LCD_Write_String("Desarmado");
	printf("Desarmado\n");
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
					printf("Estado: DESARMADO\n");
					led_des_on();
					led_arm_off();
					buzz_off();
					break;
				case ARMADO:
					LCD_Clear();
					LCD_Write_String("Armado");
					printf("Estado: ARMADO\n");
					led_arm_on();
					led_des_off();
					buzz_off();
					break;
				case CONFIG:
					printf("Estado: CONFIG\n");
					break;
			}
		}

		switch (estado) {
			case DESARMADO:
			{
				static char ultimas[3] = {0, 0, 0};
				char digito_buf[TAM];

				tecla = teclado_get_key();
				if (tecla != 0) {
					ultimas[0] = ultimas[1];
					ultimas[1] = ultimas[2];
					ultimas[2] = tecla;

					if (tecla >= '0' && tecla <= '9') {
						uint8_t n = coletar_digitos(digito_buf, TAM, tecla);
						if (n == TAM) {
							ler_senha(senha_eeprom);
							if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
								printf("\nSenha correta - Armando\n");
								estado = ARMADO;
							} else {
								LCD_Clear();
								LCD_Write_String("Senha Incorreta!");
								printf("\nSenha Incorreta!\n");
								_delay_ms(1500);
							}
						} else {
							LCD_Clear();
							LCD_Write_String("Desarmado");
							printf("Desarmado\n");
						}
					} else if (ultimas[0] == 'A' && ultimas[1] == 'B' && ultimas[2] == 'A') {
						printf("Sequencia ABA detectada - Config\n");
						estado = CONFIG;
					}
				}
				break;
			}

			case ARMADO:
			{
				char digito_buf[TAM];

				if (!alarme_ativo && PIR_Checar()) {
					alarme_ativo = 1;
					alarme_inicio = millis();
					ultimo_blink = millis();
					buzz_on();
					LCD_Clear();
					LCD_Write_String("ALARME!");
					printf("ALARME!\n");
				}

				if (alarme_ativo) {
					if (millis() - alarme_inicio >= TEMPO_ALARME) {
						alarme_ativo = 0;
						buzz_off();
						LCD_Clear();
						LCD_Write_String("Armado");
						printf("Armado\n");
						led_arm_on();
						led_des_off();
					}
					if (millis() - ultimo_blink >= 500) {
						ultimo_blink = millis();
						PORTB ^= (1 << PB4) | (1 << PB5);
					}
				}

				tecla = teclado_get_key();
				if (tecla >= '0' && tecla <= '9') {
					uint8_t n = coletar_digitos(digito_buf, TAM, tecla);
					if (n == TAM) {
						ler_senha(senha_eeprom);
						if (memcmp(digito_buf, senha_eeprom, TAM) == 0) {
							printf("\nSenha correta - Desarmando\n");
							alarme_ativo = 0;
							buzz_off();
							estado = DESARMADO;
						} else {
							LCD_Clear();
							LCD_Write_String("Senha Incorreta!");
							printf("\nSenha Incorreta!\n");
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
				printf("Nova senha (6):\n");
				if (coletar_digitos(senha1, TAM, 0) != TAM) {
					estado = DESARMADO;
					break;
				}

				LCD_Clear();
				LCD_Write_String("Confirme:");
				printf("\nConfirme:\n");
				if (coletar_digitos(senha2, TAM, 0) != TAM) {
					estado = DESARMADO;
					break;
				}

				if (memcmp(senha1, senha2, TAM) == 0) {
					setar_senha(senha1);
					LCD_Clear();
					LCD_Write_String("Senha alterada!");
					printf("\nSenha alterada!\n");
					_delay_ms(1500);
				} else {
					printf("Senhas nao conferem\n");
					LCD_Clear();
					LCD_Write_String("Erro!");
					printf("\nErro!\n");
					_delay_ms(1500);
				}

				estado = DESARMADO;
				break;
			}
		}
	}
}
