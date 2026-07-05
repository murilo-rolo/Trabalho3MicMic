#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdint.h>

#define TAMANHO_SENHA 6
#define ENDERECO_SENHA 0
#define ENDERECO_FLAG  6
#define MAGIC_FLAG     0xAA

void setar_senha(const char *nova_senha);
void ler_senha(char *buffer);
uint8_t senha_existe(void);

#endif