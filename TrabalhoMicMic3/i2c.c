#include "i2c.h"
#include "avr/io.h"

void I2C_Init(void){
	TWSR = 0x00; //Configura o prescaler para 1 (bits TWP0 e TWP1 em 0)
	//Calcula o bit rate para 100kHz com clock de 16MHz
	// 100000 = 16000000/(16+2 *TWBR*1) => TWBR =72
	TWBR = 72;
	
	//Habilita o TWI
	TWCR = (1<<TWEN);
	
}

void I2C_Start(void){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Limpa a flag de TWINT, envia bit de start (TWSTA), Habilita o I2C
	//Fica em espera até o hardware sentar a flag TWINT
	while (!(TWCR & (1 << TWINT)));
}

void I2C_Stop(void) {
	// Limpa TWINT, envia bit de stop e habilita o I2C
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	
}

void I2C_Write(uint8_t data){
	//Coloca os dados no Registrador de dados
	TWDR = data;
	
	//Limpa	TWINT e habilita I2C
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Espera até a transmissão ser concluída
	while(!(TWCR & (1<<TWINT)));
}