#include <avr/io.h>  
#include <pololu/orangutan.h>  


void i2c_init()
{
	DDRC = 0;                              // all inputs
	PORTC = (1 << PORTC1) | (1 << PORTC0); // enable pull-ups on SDA and SCL, respectively

	TWSR = 0;  // clear bit-rate prescale bits
	TWBR = 17; // produces an SCL frequency of 400 kHz with a 20 MHz CPU clock speed
}

void i2c_start() {  
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // send start condition  
	while (!(TWCR & (1 << TWINT)));  
}  
  
void i2c_write_byte(char byte) {  
	TWDR = byte;              
	TWCR = (1 << TWINT) | (1 << TWEN); // start address transmission  
	while (!(TWCR & (1 << TWINT)));  
}  
  
char i2c_read_byte() {  
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN); // start data reception, transmit ACK  
	while (!(TWCR & (1 << TWINT)));  
	return TWDR;  
}  

char i2c_read_last_byte() {  
	TWCR = (1 << TWINT) | (1 << TWEN); // start data reception
	while (!(TWCR & (1 << TWINT)));  
	return TWDR;  
}  
  
void i2c_stop() {  
	  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // send stop condition  
}  

