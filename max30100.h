// Defines Gerais
#define MAX30100_ADDRESS 0x57						// MAX30100 Address
#define NUM_SAMPLES_TO_READ 450						// samples rate from Max30100 buffer - samples/s

// FIFO control and data registers
#define MAX30100_REG_FIFO_WRITE_POINTER         0x02
#define MAX30100_REG_FIFO_OVERFLOW_COUNTER      0x03
#define MAX30100_REG_FIFO_READ_POINTER          0x04
#define MAX30100_REG_FIFO_DATA                  0x05  // Burst read does not auto-increment addr

// Mode Configuration register
#define MAX30100_REG_MODE_CONFIGURATION         0x06
#define MAX30100_MC_TEMP_EN                     (1 << 3)
#define MAX30100_MC_RESET                       (1 << 6)
#define MAX30100_MC_SHDN                        (1 << 7)
typedef enum Mode {
	MAX30100_MODE_HRONLY    = 0x02,
	MAX30100_MODE_SPO2_HR   = 0x03
} Mode;

#define MAX30100_REG_SPO2_CONFIGURATION         0x07
#define MAX30100_SPC_SPO2_HI_RES_EN             (1 << 6)
typedef enum SamplingRate {
	MAX30100_SAMPRATE_50HZ      = 0x00,
	MAX30100_SAMPRATE_100HZ     = 0x01,
	MAX30100_SAMPRATE_167HZ     = 0x02,
	MAX30100_SAMPRATE_200HZ     = 0x03,
	MAX30100_SAMPRATE_400HZ     = 0x04,
	MAX30100_SAMPRATE_600HZ     = 0x05,
	MAX30100_SAMPRATE_800HZ     = 0x06,
	MAX30100_SAMPRATE_1000HZ    = 0x07
} SamplingRate;

void max_init()
{
	// Set FIFO_WR_PTR to 0
	print_Serial_str("max;");
	_delay_ms(5);
	i2c_start();
	_delay_ms(5);
	print_Serial_str("star;");
	i2c_write_address(0xAE);
	i2c_write_data(0x02);
	i2c_write_data(0x00);
	i2c_stop();
	
	_delay_ms(100);
	
	// Set OV_COUNTER to 0
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x03);
	i2c_write_data(0x00);
	i2c_stop();
	
	_delay_ms(100);
	
	// Set FIFO_RD_PTR to 0
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x04);
	i2c_write_data(0x00);
	i2c_stop();
	
	_delay_ms(100);
	
	// Configuring Mode of MAX30100
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x06);				//-- Mode Configuration
	i2c_write_data(0x03);				//-- Enable Heart Rate Mode Only
	i2c_stop();
	
	_delay_ms(100);
	
	// Led pulse width control
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x07);				//-- SpO2 Configuration
	i2c_write_data(0x11);				//-- ADC-14 SAMPLES-400 0b'0001 0001
	i2c_stop();
	
	_delay_ms(100);
	
	// Led Configuration
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x09);				//-- Led Configuration
	i2c_write_data(0x77);				//-- 24mA Spo2 and HR
	i2c_stop();
	
	_delay_ms(100);
}

void max_read_hr()
{
	
}

uint8_t max_read_ID()
{
	uint8_t address;
	i2c_start();
	i2c_write_address(MAX30100_ADDRESS);
	i2c_write_data(0xFF);				//-- Part ID
	address = i2c_read();
	i2c_stop();
	return address;
}

uint8_t max_write_pointer()
{
	uint8_t fifo_write;
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x02);
	i2c_re_start();
	i2c_read_address(0xAF);
	fifo_write = i2c_read_noack();
	i2c_stop();
	return fifo_write;
}

uint8_t max_read_pointer()
{
	uint8_t fifo_read;
	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x04);
	i2c_re_start();
	i2c_read_address(0xAF);
	fifo_read = i2c_read_noack();
	i2c_stop();
	return fifo_read;
}

int32_t max_read_data(uint16_t* infrared, uint16_t* red)
{
	int i;
	int32_t numAvaliableSamples;
	int32_t wrPtr, rdPtr;
	
	wrPtr = max_write_pointer();
	// 	print_Serial_str(", wrPTR ");
	// 	print_Serial_signed16(wrPtr);
	_delay_ms(5);
	rdPtr = max_read_pointer();
	// 	print_Serial_str(", rdPTR ");
	// 	print_Serial_signed16(rdPtr);
	_delay_ms(5);
	numAvaliableSamples = wrPtr - rdPtr;
	if(numAvaliableSamples < 0) numAvaliableSamples = 16 + numAvaliableSamples;
	// 	print_Serial_str(", dif ");
	// 	print_Serial_signed16(numAvaliableSamples);
	_delay_ms(1);

	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x05);
	i2c_re_start();
	i2c_read_address(0xAF);
	for(i=0; i < numAvaliableSamples-1; i++)
	{
		*infrared = i2c_read();
		*infrared = (*infrared << 8);
		*infrared |= i2c_read();
		
		*red = i2c_read();
		*red = (*red << 8);
		*red |= i2c_read();
		
		infrared++;
		red++;
	};
	
	*infrared = i2c_read();
	*infrared = (*infrared << 8);
	*infrared |= i2c_read();
	
	*red = i2c_read();
	*red = (*red << 8);
	*red |= i2c_read_noack();
	
	i2c_stop();
	
	return numAvaliableSamples;
}