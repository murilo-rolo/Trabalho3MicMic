#include "eeprom.h"

void setar_senha(const char *nova_senha)
{
	for (uint8_t i = 0; i < TAMANHO_SENHA; i++) {
		eeprom_write_byte((uint8_t*)(ENDERECO_SENHA + i), nova_senha[i]);
	}
	if (eeprom_read_byte((uint8_t*)ENDERECO_FLAG) != MAGIC_FLAG) {
		eeprom_write_byte((uint8_t*)ENDERECO_FLAG, MAGIC_FLAG);
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
	return eeprom_read_byte((uint8_t*)ENDERECO_FLAG) == MAGIC_FLAG;
}