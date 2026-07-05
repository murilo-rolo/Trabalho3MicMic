#ifndef LCD_H
#define LCD_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <stdint.h>
#include "i2c.h"
#include <util/delay.h>

// Endere�o I2C do m�dulo LCD (0x27 deslocado 1 bit para a esquerda)
// Se o seu display n�o funcionar, tente mudar para 0x7E (que � o 0x3F deslocado)
#define LCD_ADDR 0x4E

// Fun��es da biblioteca
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Clear(void);
void LCD_Write_Char(char data);
void LCD_Write_String(const char *str);
void LCD_Ponteiro(uint8_t linha, uint8_t coluna);

#endif /* LCD_H */