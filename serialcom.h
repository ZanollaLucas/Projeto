/*
  * serialcom.h
  * 
  * Created: 20/03/2019
  * Author : LUCAS ZANOLLA
  */


void init_Serial(uint16_t baud)
{
	UBRR0 = 0x0000;
	UBRR0H = (baud>>8);                      // shift the register right by 8 bits
	UBRR0L = baud;                           // set baud rate
	UBRR0 |= 0x8000;
	UCSR0B|= (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);                // enable receiver and transmitter and interrupt from receiver
	UCSR0C|= (1<<UCSZ00)|(3<<UCSZ01);
}

void print_Serial_str(unsigned char buffer[])
{
	uint8_t i = 0;
	
	while(buffer[i] != '\0')
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = buffer[i];
		i++;
	}
}

void print_Serial_char(unsigned char buffer)
{
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = buffer;
}

unsigned char read_Serial()
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}

void print_Serial_uint8(uint8_t value)
{
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = value;
}


void print_Serial_uint16(uint16_t value)
{
	uint16_t valor;
	uint16_t anterior;
	uint8_t zeroprint = 1;

	valor = value/10000;
	if(!valor) zeroprint = 0;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (value - (valor*10000));
	valor = anterior/1000;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*1000));
	valor = anterior/100;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*100));
	valor = anterior/10;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*10));
	valor = anterior;
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
}

void print_Serial_int16(int value)
{
	uint16_t valor;
	uint16_t anterior;
	uint8_t zeroprint = 1;
	
	if (valor < 0 )
	{
		valor = valor * (-1);
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x2D;								// 0x2D para sinal -
	}

	valor = value/10000;
	if(!valor) zeroprint = 0;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (value - (valor*10000));
	valor = anterior/1000;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*1000));
	valor = anterior/100;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*100));
	valor = anterior/10;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*10));
	valor = anterior;
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
}

void print_Serial_signed16(int32_t value)
{
	uint16_t valor;
	uint16_t anterior;
	uint8_t zeroprint = 1;

	if(value < 0)
	{
		value = value*(-1);
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x2D;
	}
	
		valor = value/10000;
		if(!valor) zeroprint = 0;
		if(zeroprint)
		{
			while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
			UDR0 = 0x30 + valor;
		}
		
		anterior = (value - (valor*10000));
		valor = anterior/1000;
		if(valor) zeroprint = 1;
		if(zeroprint)
		{
			while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
			UDR0 = 0x30 + valor;
		}
		
		anterior = (anterior - (valor*1000));
		valor = anterior/100;
		if(valor) zeroprint = 1;
		if(zeroprint)
		{
			while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
			UDR0 = 0x30 + valor;
		}
		
		anterior = (anterior - (valor*100));
		valor = anterior/10;
		if(valor) zeroprint = 1;
		if(zeroprint)
		{
			while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
			UDR0 = 0x30 + valor;
		}
		
		anterior = (anterior - (valor*10));
		valor = anterior;
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
		
}

void print_Serial_float(float value)
{
	float decimal;
	int32_t inteiro;
	int32_t valor;
	int32_t anterior;
	uint8_t zeroprint = 1;
	//uint8_t negativoprint = 0;
	
	
	if(value < 0)
	{
		//negativoprint = 1;
		value = value*(-1);
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x2D;
	}
	inteiro = value;
	valor = value/10000;
	if(!valor) zeroprint = 0;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));            // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (value - (valor*10000));
	valor = anterior/1000;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*1000));
	valor = anterior/100;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*100));
	valor = anterior/10;
	if(valor) zeroprint = 1;
	if(zeroprint)
	{
		while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
		UDR0 = 0x30 + valor;
	}
	
	anterior = (anterior - (valor*10));
	valor = anterior;
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
	
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x2E;
	
	decimal = (value - inteiro) * 1000;
	valor = decimal/100;
	anterior = (decimal - (valor*100));
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
	
	valor = anterior/10;
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
	
	valor = (anterior - (valor*10));
	while (!( UCSR0A & (1<<UDRE0)));                // wait while register is free
	UDR0 = 0x30 + valor;
}
