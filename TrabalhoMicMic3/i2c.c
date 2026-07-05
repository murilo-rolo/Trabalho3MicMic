#include "i2c.h"

void I2C_Init(void){
	TWSR = 0x00; //Configura o prescaler para 1 (bits TWP0 e TWP1 em 0)
	//Calcula o bit rate para 100kHz com clock de 16MHz
	// 100000 = 16000000/(16+2 *TWBR*1) => TWBR =72
	TWBR = 72;
	
	//Habilita o TWI
	TWCR = (1<<TWEN);
	
}

static uint8_t I2C_Aguardar(void)
{
	uint16_t timeout = 0;
	while (!(TWCR & (1 << TWINT))) {
		if (++timeout > 2000) return 1;
	}
	return 0;
}

void I2C_Start(void){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	I2C_Aguardar();
}

void I2C_Stop(void) {
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

uint8_t I2C_Write(uint8_t data){
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	if (I2C_Aguardar()) return 1;
	if ((TWSR & 0xF8) != 0x28) return 1;
	return 0;
}