#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "VirtualSerial.h"

#define cs_lo PORTC &= ~(1<<PORTC7)
#define cs_hi PORTC |= (1<<PORTC7)

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

void init_SPI()
{
	DDRB |= ((1<<DDB0)|(1<<DDB2)|(1<<DDB1));
	DDRB &= ~(1<<DDB3);
	DDRC |= (1<<DDC7);

	SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
	SPSR &= ~(1<<SPI2X);
}

char sd_raw_send_byte(char b)
{
	SPDR = b;
	while (!(SPSR&(1<<SPIF)));
	SPSR &= ~(1<<SPIF);
	return SPDR;
}

void binaire(char c, char *o)
{
	//[... fonction qui remplit o avec les caracteres 0 ou 1 de la sequence binaire de c ...]
	
	int i;
	
	for (i = 0; i < 8; i++)
	{
		o[7-i] = ((c>>i)&0x01)+'0';
	}
}

int main()
{
	char buffer[80] = "C'est parti\r\n\0";
	unsigned char ch, cl;
	int k;
	SetupHardware();
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	GlobalInterruptEnable();

	DDRB |= 1<<PB4;
	PORTB &= ~(1<<PB4);
	init_SPI();

	fputs(buffer, &USBSerialStream);

	while(1)
	{
		cs_lo;
		ch = sd_raw_send_byte(0x00);
		binaire(ch, buffer);
		cl = sd_raw_send_byte(0x00);
		binaire(cl, &buffer[8]);
		for (k=0; k<2; k++)
		{
			sd_raw_send_byte(0x00);
		}
		cs_hi;
		buffer[16] = '\r';
		buffer[17] = '\n';
		buffer[18] = 0;
		fputs(buffer, &USBSerialStream);

		_delay_ms(100);
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
	return(0);
}
