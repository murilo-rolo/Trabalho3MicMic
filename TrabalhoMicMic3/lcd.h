#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Endereço I2C do módulo LCD (0x27 deslocado 1 bit para a esquerda)
// Se o seu display não funcionar, tente mudar para 0x7E (que é o 0x3F deslocado)
#define LCD_ADDR 0x4E

// Funções da biblioteca
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Clear(void);
void LCD_Write_Char(char data);
void LCD_Write_String(char *str);
void LCD_Ponteiro(uint8_t linha, uint8_t coluna);

#endif /* LCD_H */