#include <avr/io.h>

#ifndef EEPROM_H_
#define EEPROM_H_

#define TAMANHO_SENHA 6

void setar_senha(const char *nova_senha);
void ler_senha(char *buffer);
uint8_t senha_existe(void);

#endif