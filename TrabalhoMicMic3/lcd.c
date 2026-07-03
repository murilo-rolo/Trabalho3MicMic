#include "lcd.h"
#include "i2c.h"
#define F_CPU 16000000UL // Assumindo que está a usar 16 MHz
#include <util/delay.h>

//Bits de controle do PCF8574
#define LCD_BACKLIGHT 0X08
#define LCD_ENABLE 0x04
#define LCD_COMMAND 0x00
#define LCD_DATA 0x01

//Função que envia 1 byte direto do I2C para o chip
void _LCD_Write_I2C(uint8_t data){
	I2C_Start();
	I2C_Write(LCD_ADDR);
	I2C_Write(data| LCD_BACKLIGHT); //Backlight sempre ligado
	I2C_Stop();
}

//Função que envia 4 bits e dá um pulso no pino EN
void _LCD_Write_Half(uint8_t half, uint8_t mode){
	uint8_t data_i2c = (half & 0xF0) | mode;
	
	//Envia os dados com EN no alto
	_LCD_Write_I2C(data_i2c | LCD_ENABLE);
	_delay_us(1);
	
	//Envia os dados com EN no baixo
	_LCD_Write_I2C(data_i2c & ~LCD_ENABLE);
	_delay_us(50);
}

// Função que envia comandos (configurações) de 8 bits para o LCD
void LCD_Command(uint8_t cmd){
	// 4 bits mais altos são enviados no modo COMANDO
	_LCD_Write_Half(cmd & 0xF0, LCD_COMMAND);
	// 4 bits mais baixos são jogados pra esquerda e enviados
	_LCD_Write_Half((cmd << 4) & 0xF0, LCD_COMMAND);
}

void LCD_Write_Char(char data){
	//4 bits mais altos são enviados no modo DADO
	_LCD_Write_Half(data & 0xF0, LCD_DATA);
	//4 bits mais baixos são jogados pra esquerda e enviados
	_LCD_Write_Half((data<<4) & 0xF0, LCD_DATA);
}

void LCD_Init(void){
	_delay_ms(50);
	
	//Sequencia de forçar reinício
	_LCD_Write_Half(0x30, LCD_COMMAND);
	_delay_ms(5);
	_LCD_Write_Half(0x30, LCD_COMMAND);
	_delay_ms(150);
	_LCD_Write_Half(0x30,LCD_COMMAND);
	
	//Diz para o LCD trabalhar no modo 4 bits
	_LCD_Write_Half(0x20,LCD_COMMAND);
	_delay_ms(1);
	
	
	LCD_Command(0x28); // Modo 4 bits, 2 linhas, fonte 5x8
	LCD_Command(0x0C); // Liga o display e esconde o cursor
	LCD_Command(0x06); // Modo de incremento
	
	LCD_Clear(); //Limpa a tela	
}

void LCD_Clear(void){
	LCD_Command(0x01); //Comando de clear display
	_delay_ms(2);
}



void LCD_Write_String(char *str){
	int i;
	
	//Laço de percorrer a palavra até \0
	for(i=0;str[i] != '\0';i++){
		LCD_Write_Char(str[i]);
	}
}

void LCD_Ponteiro(uint8_t linha, uint8_t coluna){
	uint8_t address;
	
	if(linha == 0){
		address = 0x80; // Seta o endereço base como inicio da linha 1
	} else{
		address = 0xC0; //Seta o endereço base como inicio da linha 2
	}
	LCD_Command(address+coluna); // soma endereço base com coluna escolhida e envia o comando
}


