#include "eeprom.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include "eeprom.h"

#define ENDERECO_SENHA 0

static const char senha_fabrica[TAMANHO_SENHA] = "123456";

void setar_senha(const char *nova_senha)
{
	for (uint8_t i = 0; i < TAMANHO_SENHA; i++) {
		eeprom_write_byte((uint8_t*)(ENDERECO_SENHA + i), nova_senha[i]);
	}
}

void ler_senha(char *buffer)
{
	for (uint8_t i = 0; i < TAMANHO_SENHA; i++) {
		buffer[i] = eeprom_read_byte((uint8_t*)(ENDERECO_SENHA + i));
	}
}

uint8_t senha_existe(void)
{
	char senha_atual[TAMANHO_SENHA];
	ler_senha(senha_atual);

	for (uint8_t i = 0; i < TAMANHO_SENHA; i++) {
		if (senha_atual[i] != senha_fabrica[i]) {
			return 1; // diferente da de fabrica -> existe senha customizada
		}
	}
	return 0; // igual a de fabrica -> ainda nao foi trocada
}