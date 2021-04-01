//-- I2C Library --//


void i2c_init()
{
	TWBR = 0x00;
	TWBR = 0x20;			//-- Clock Frequency = 200KHz
	TWSR = 0x00;			//-- Prescaler = 1
}

void i2c_start()
{	
	//print_Serial_str("i2c");
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTA);	//-- Start Condition
	//print_Serial_str("starcond;");
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	//print_Serial_str("ae!");
	if((TWSR & 0xF8) != 0x08){
		//print_Serial_str("again;");
		 i2c_start();	//-- Start Condition Status
	}
}

void i2c_re_start()
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTA);	//-- Start Condition
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	if((TWSR & 0xF8) != 0x10) i2c_re_start();	//-- Repeated Start Condition Status
}

void i2c_stop()
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);	//-- Stop Condition
}

void i2c_write_address(uint8_t address)
{
	TWDR = address;				//-- Move value to I2C Data Register
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	if((TWSR & 0xF8) != 0x18) i2c_write_address(address);	//-- SLA+W and ACK received
}

void i2c_write_data(uint8_t data)
{
	TWDR = data;				//-- Move value to I2C Data Register
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	if((TWSR & 0xF8) != 0x28) i2c_write_data(data);	//-- Data byte transmitted and ACK received
}

void i2c_write_data_noack(uint8_t data)
{
	TWDR = data;				//-- Move value to I2C Data Register
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	if((TWSR & 0xF8) != 0x30) i2c_write_data_noack(data);	//-- Data byte transmitted and NOT ACK received
}

void i2c_read_address(uint8_t address)
{
	TWDR = address;				//-- Move value to I2C Data Register
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	if((TWSR & 0xF8) != 0x40) i2c_read_address(address);	//-- SLA+R and ACK received
}

uint8_t i2c_read()
{
	uint8_t ret;
	TWCR |= (1 << TWINT)|(1 << TWEA)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	ret = TWDR;
	if((TWSR & 0xF8) != 0x50) ret = i2c_read();	//-- Data received ACK returned
	return ret;				//-- Return value from I2C Data Register
}

uint8_t i2c_read_noack()
{
	uint8_t ret;
	TWCR |= (1 << TWINT)|(1 << TWEN);
	TWCR &= ~(1 << TWEA);			//-- Disable Acknowledge
	while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	ret = TWDR;
	if((TWSR & 0xF8) != 0x58) ret = i2c_read_noack();	//-- Data received NOT ACK returned
	return ret;					//-- Return value from I2C Data Register
}

void i2c_init_slave(uint8_t address)
{
	TWAR = address;
}

void i2c_match_ack()
{
	while((TWSR & (0xF8)) != 0x60)	//-- SLA+W, ACK returned
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
		while(!(TWCR & (1 << TWINT)));	//-- Waiting complete operation
	}
}