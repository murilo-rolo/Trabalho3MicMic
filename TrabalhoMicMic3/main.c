/*
 * TrabalhoMicMic3.c
 *
 * Created: 23/06/2026 15:59:07
 * Author : muril
 */ 

#define F_CPU 16000000UL // Avisa o compilador que estamos em 16 MHz
#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "lcd.h"

int main(void) {
	// 1. Inicializa o hardware do I2C
	I2C_Init();
	
	// 2. Inicializa o software do LCD
	LCD_Init();
	
	// 3. Escreve na tela!
	LCD_Ponteiro(0, 3); // Linha 0 (primeira), Coluna 3
	LCD_Write_String("Alerta!");
	
	LCD_Ponteiro(1, 0); // Linha 1 (segunda), Coluna 0
	LCD_Write_String("Sem perigo!");
	
	while(1) {
		// Seu código principal roda aqui livremente
	}
	
	return 0;
}

