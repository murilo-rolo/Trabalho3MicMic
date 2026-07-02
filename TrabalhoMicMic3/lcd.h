#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>

// Endereço I2C do módulo LCD (0x27 deslocado 1 bit para a esquerda)
// Se o seu display não funcionar, tente mudar para 0x7E (que é o 0x3F deslocado)
#define LCD_ADDR 0x4E

// Funções da biblioteca
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Char(char data);
void LCD_String(char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

#endif /* LCD_I2C_H */