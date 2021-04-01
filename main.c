/*
 * HR+ Spo2.c
 *
 * Created: 27/11/2019 17:44:59
 * Author : LucasZanolla
 */ 

#define F_CPU 16000000UL
#define BAUD 9600                                  // define baud
#define BAUDRATE ((F_CPU)/(BAUD*8UL)-1)            // set baud rate value for UBRR

// Filters Defines
#define ALPHA 0.95
#define OMEGA 0.04		// Sugestão de uso para 1/metade do NUM_SAMPLES -> 1/25
#define A 0.00207758
#define B 1.8669906
#define C 0.875301


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/interrupt.h>
#include "C:\Users\LucasZanolla\Documents\Atmel Studio\7.0\headers\serialcom.h"
#include "C:\Users\LucasZanolla\Documents\Atmel Studio\7.0\headers\twicom.h"
#include "C:\Users\LucasZanolla\Documents\Atmel Studio\7.0\headers\max30100.h"

uint16_t red[NUM_SAMPLES_TO_READ];
uint16_t infrared[NUM_SAMPLES_TO_READ];
int32_t anteriorInfra;
int32_t anterioRed;
uint8_t BPM_INIT;
uint8_t BPM_END;
uint8_t BPM_RDY;
volatile int32_t TOVERFLOW;
volatile int32_t THRESHOLDING = 0;

uint8_t read_bpm(float sample, int threshold);
int max_read(int k);

int32_t dc_filter(int32_t x, int32_t w_prev, float alpha);
int32_t dc_filter2(int32_t x, int32_t w_prev, float alpha);
int32_t avg_exp_filter(int32_t sample, float w, int32_t mean);
float low_pass_filter(int32_t sample, int32_t x1, int32_t x2, float y1, float y2, float a, float b, float c);

const uint8_t spO2LUT[43] = {100,100,99,99,99,99,98,98,98,98,98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,95,95,95,94,
94,94,94,94,93,93,93,93,92,92,91,90,89,88};



void timer0_init();

int main(void)
{
	cli();
	uint16_t baud = BAUDRATE;
	int32_t filtInfra[NUM_SAMPLES_TO_READ];
	int32_t avgInfra[NUM_SAMPLES_TO_READ];
	float butterInfra[NUM_SAMPLES_TO_READ];
	int32_t sampInfra[2] = {0, 0};
	float resultInfra[2] = {0.0, 0.0};
	int32_t filtRed[NUM_SAMPLES_TO_READ];
	int32_t irACValueSqSum, redACValueSqSum;
	int32_t mean;
	// 	int32_t samplesReaded;
	// 	int32_t samplesCounter = 0;
	int i, j, k;
	uint8_t ind;
	uint16_t bpm[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	float Ratio[8];
	//uint16_t bpm[60];
	int bpmCounter = 0;
	uint8_t acum = 0;
	float tempo;

	// -- Flags Initialization
	BPM_INIT = 0;
	BPM_END = 0;
	BPM_RDY = 0;
	
	// -- Modules Initialization
	_delay_ms(5);
	i2c_init();
	_delay_ms(5);
	init_Serial(BAUDRATE);
	_delay_ms(50);
	print_Serial_str("oi!");
	max_init();
	print_Serial_str("a;");
	//timer0_init();
	sei();
	
	infrared[0] = 0;
	anteriorInfra = 0;
	red[0] = 0;
	anterioRed = 0;
	
	while (1)
	{
	//	print_Serial_str("while1; ");
		i = 0;
		j = 0;
		k = 0;
		while(i<400)
		{
			j = max_read(i);
			//print_Serial_str("leitura; ");
			for(k=i; k<=j; k++)
			{
			//	print_Serial_str("filtros; ");
				filtInfra[k] = dc_filter((int32_t)infrared[k], anteriorInfra, ALPHA);
				filtRed[k] = dc_filter2((int32_t)red[k], anterioRed, ALPHA);
			//	print_Serial_str("media; ");
				if(!k) mean = filtInfra[k];
				avgInfra[k] = avg_exp_filter(filtInfra[k], OMEGA, mean);
				mean = avgInfra[k];
			//	print_Serial_str("Butter; ");
				butterInfra[k] = low_pass_filter(avgInfra[k], sampInfra[0], sampInfra[1], resultInfra[0], resultInfra[1], A, B, C);
				resultInfra[1] = resultInfra[0];
				resultInfra[0] = butterInfra[k];
				sampInfra[1] = sampInfra[0];
				sampInfra[0] = avgInfra[k];
			//	print_Serial_str("filtOK; ");
				if(read_bpm(butterInfra[k], THRESHOLDING))
				{
					if(BPM_RDY)
					{
						bpm[acum] = 60/(bpmCounter*(1.0/NUM_SAMPLES_TO_READ));
						//print_Serial_str("ACV: ");
						//print_Serial_signed16(redACValueSqSum);
						Ratio[acum] = 100* ((log(redACValueSqSum)) / (log(irACValueSqSum)));
						BPM_RDY = 0;
						bpmCounter = 0;
						redACValueSqSum = 0;
						irACValueSqSum = 0;
											
						
						if(bpm[acum]>=35 && bpm[acum]<=180)
						{
							if (acum == 0)
							{
								print_Serial_uint16((bpm[acum]));
								print_Serial_str(";");
								print_Serial_signed16(THRESHOLDING);
								print_Serial_str(";");
								//print_Serial_float( Ratio[acum]);
								acum++;
							}
							else if ((abs(bpm[acum]-bpm[acum-1]) <= 30))
							{
								print_Serial_uint16((bpm[acum]));
								print_Serial_str(";");
								print_Serial_signed16(THRESHOLDING);
								print_Serial_str(";");
								//print_Serial_float( Ratio[acum]);
								acum++;							
							}
							
						}
						else if(bpm[acum] <= 35)
						{
							THRESHOLDING += 20;
							print_Serial_signed16(THRESHOLDING);
							print_Serial_str("//");
						}
						else if(bpm[acum] >= 180)
						{
							THRESHOLDING -= 20;
							print_Serial_signed16(THRESHOLDING);
							print_Serial_str("//");
						}
					}
					else
					{
						BPM_RDY = 1;
					}
					if(acum >= 8)
					{
						for(j=1; j<8; j++)
						{
							bpm[0] += bpm[j];
							Ratio[0] += Ratio[j];
						}
						
						Ratio[0] = Ratio[0]/8;
						/*print_Serial_str("Ratio: ");
						print_Serial_float(Ratio[0]);*/
						
						print_Serial_str(" Média: ");
						print_Serial_uint16((bpm[0]/8));
						print_Serial_str(" BPM - Spo2: ");
						
						if (Ratio[0] > 43) {
							ind = (int) Ratio % 43;
							print_Serial_signed16(spO2LUT[ind]);
						}
						else
						{
							print_Serial_uint16(spO2LUT[ind]);
						}
						print_Serial_str("%");
						print_Serial_str(";");
						
						//_delay_ms(10000);
						
						acum = 0;
					}
				}
				if(BPM_RDY)
				{
					bpmCounter++;
					irACValueSqSum = ((abs(filtInfra[k]) + irACValueSqSum)/2);
					redACValueSqSum = ((abs(filtRed[k]) + redACValueSqSum)/2);
				}
			}
		
		}
	}
	return 0;
}

uint8_t read_bpm(float sample, int threshold)
{
	if(sample < threshold && !BPM_INIT && !BPM_END)
	{
		BPM_INIT = 1;
		BPM_END = 0;
		return 1;
	}
	
	if(sample > threshold && BPM_INIT && !BPM_END)
	{
		BPM_INIT = 0;
		BPM_END = 1;
		return 0;
	}
	
	if(sample < threshold && !BPM_INIT && BPM_END)
	{
		BPM_INIT = 1;
		BPM_END = 1;
		return 0;
	}
	
	if(sample > threshold && BPM_INIT && BPM_END)
	{
		BPM_INIT = 0;
		BPM_END = 0;
		return 1;
	}
	return 0;
}

int max_read(int k)
{
	uint8_t wrPtr, rdPtr;
	int32_t numAvaliableSamples;
	int i;

	wrPtr = max_write_pointer();
	_delay_ms(5);
	rdPtr = max_read_pointer();
	_delay_ms(5);
	numAvaliableSamples = wrPtr - rdPtr;
	if(numAvaliableSamples < 0) numAvaliableSamples = 16 + numAvaliableSamples;

	i2c_start();
	i2c_write_address(0xAE);
	i2c_write_data(0x05);
	i2c_re_start();
	i2c_read_address(0xAF);
	for(i=0; i < numAvaliableSamples-1; i++)
	{
		infrared[k] = i2c_read();
		infrared[k] = (infrared[k] << 8);
		infrared[k] |= i2c_read();
		
		red[k] = i2c_read();
		red[k] = (red[k] << 8);
		red[k] |= i2c_read();
		k++;
	};
	
	infrared[k] = i2c_read();
	infrared[k] = (infrared[k] << 8);
	infrared[k] |= i2c_read();
	
	red[k] = i2c_read();
	red[k] = (red[k] << 8);
	red[k] |= i2c_read_noack();
	
	i2c_stop();
	
	return k;
}

int32_t dc_filter(int32_t x, int32_t w_prev, float alpha) {
	int32_t w;
	int32_t y;

	w = x + alpha * w_prev;
	y = w - w_prev;
	
	anteriorInfra = w;
	
	return y;
}

int32_t dc_filter2(int32_t x, int32_t w_prev, float alpha) {
	int32_t w;
	int32_t y;

	w = x + alpha * w_prev;
	y = w - w_prev;
	
	anterioRed = w;
	
	return y;
}

int32_t avg_exp_filter(int32_t sample, float w, int32_t mean) {

	sample = w * sample * 5 + (1 - w) * mean;


	return sample;
}

float low_pass_filter(int32_t sample, int32_t x1, int32_t x2, float y1, float y2, float a, float b, float c)
{
	float y;
	y = a*(sample + 2*x1 + x2) + b*y1 - c*y2;
	return y;
}