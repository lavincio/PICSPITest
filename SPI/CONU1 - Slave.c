/*
** CONIO.C
** Basic CONSOLE I/O for Explorer16 demonstration board
** PIC32 v 2.0 LDJ 7/18/07
** Modified for Cerebot 32MX7 - R. W. Wall,  October 2, 2011
** Uses PBDIV = 8, SYS_OSC = 80,000,000
*/
//update by LSH on 11/18/2011 added spi 3 and 4, uart 1 and 2 on 32mx7 
#include <plib.h>
#include <stdio.h>
#include <p32xxxx.h>
#include "conU1.h"
#include "Cerebot32mx7.h"
#include "config_bits(4).h"

#define BRATE		9600      		
#define U_ENABLE	0x8008			// enable the UART peripheral
#define U_TX		0x0400			// enable transmission

// Global character buffers
#define CHAR_ARR_SIZE 20
char 	rxbuf[CHAR_ARR_SIZE];
int 	rxhead = 0;
int 	rxcnt = 0;
char 	txbuf[CHAR_ARR_SIZE];
int 	txhead = 0;
int 	txcnt = 0;
//volatile char 	tempchar = 0;
char charsave[100];
volatile datacount = 0;
volatile looped = 0;		//if finihsed a loop
int spi(int Device);															//spi initialization (spi3 or spi4) 
int initUART(void);


//Device settings
//1 = Uart1 listed as uart1a on schematic pmod header JE
//2 = Uart2 listed as uart3a on schematic pmod header JF
//3 = SPI3 port listed in the schematic as SS1A,SDI1A,SDO1A,SCK1A pmod header JE
//4 = SPI4 port listed in the schematic as SS3A,SDI3A,SDO3A,SCK3A pmod header JF


int main(void)
{	

//Device settings
//1 = Uart1 listed as uart1a on schematic pmod header JE
//2 = Uart2 listed as uart3a on schematic pmod header JF
//3 = SPI3 port listed in the schematic as SS1A,SDI1A,SDO1A,SCK1A pmod header JE
//4 = SPI4 port listed in the schematic as SS3A,SDI3A,SDO3A,SCK3A pmod header JF
	int device = 3;						//this sets the interface 
	int count = 0, data = 0;
	char tempchar;
	int prev_datacount = 0;

	mPORTGSetPinsDigitalOut(BIT_12|BIT_13|BIT_14|BIT_15);

	// Enable multi-vector interrupts.
	INTEnableSystemMultiVectoredInt();

	spi(device);					
	initUART();
	
	SpiChnGetC(3);// receive data on the slave channel
	SpiChnWriteC(3, '*');

	while(1)								//demo Loop I made it simple step by step so its easier to read
	{
		data = SpiChnGetC(3);// receive data on the slave channel
		if(datacount <= prev_datacount && looped == 0)
		{
		while(!SpiChnTxBuffEmpty(3))	//checks the tx buffer if full wait 
				{}
				SpiChnWriteC(3, 0x00);				//loads the char c in to the device selected
		
		}
		else{
			while(!SpiChnTxBuffEmpty(3))	//checks the tx buffer if full wait 
				{}
				SpiChnWriteC(3, charsave[prev_datacount]);				//loads the char c in to the device selected
			U2TXREG = charsave[prev_datacount];	//load for transmit the char c
			prev_datacount++;
			if(prev_datacount == 100)
			{
				prev_datacount = 0;
				looped = 0;
			}
		}

		if(data!=0x00){
			while(!U2STAbits.TRMT | U2STAbits.UTXBF) //this checks the tx buffer if full or if the transmit is in process waits for the flags to clear
			{}
			U2TXREG = data;	//load for transmit the char c
		}	
//		count++;	
	} 

}


//******************************************************************************************************************************//
//UART functions
// Init the serial port UART2, 9600@FBR of 40MHz, 8, N, 1,
// No hardware hanshaking
// Peripheral Library function replaces text method for setting UART 
int initUART(void)
{
	unsigned int BRG;

	BRG = (unsigned short)(( (float)FPB / ( (float)4 * (float) BRATE ) ) - (float)0.5);
	//OpenUART1( UART_EN, 
	//UART_RX_ENABLE | UART_TX_ENABLE | UART_BRGH_FOUR | UART_INT_TX_BUF_EMPTY | UART_INT_RX_CHAR, BRG );
	OpenUART2( UART_EN, 
	UART_RX_ENABLE | UART_TX_ENABLE | UART_BRGH_FOUR | UART_INT_TX_BUF_EMPTY | UART_INT_RX_CHAR , BRG );

	// Set interrupt priority
	//IPC6	= 0x1f; // interrupt priority level 7, sub 3
	IPC8	= 0x17; // interrupt priority level 5, sub 3
	// Enable U1TXIF, U1RXIF
	//IFS0CLR = ( 1 << 28 | 1 << 27);
	//IEC0SET	= ( 1 << 28 | 1 << 27);
	//IFS1CLR = ( 1 << 9 | 1 << 10);
	//IEC1SET	= ( 1 << 9 | 1 << 10);
	// Enable U1RXIF
	//IFS0CLR = ( 1 << 27);
	//IEC0SET	= ( 1 << 27);
	IFS1CLR = ( 1 << 9);
	IEC1SET	= ( 1 << 9);
} 

//******************************************************************************************************************************//
//SPI interface initialization
//This sets up spi defined by Device (3 or 4) in the schematic to use the following settings
//SPI open as master using hardware Chip select control, open in 8 bit mode, turn on SPI4,clear transmit buffer empty enabled,
//open CKE in SPI mode 0, baud rate set to 10MHz/20 = 500kHx
// Peripheral Library function replaces text method for setting SPI

int spi(int Device)
{
//	SpiChnOpen(Device,SPI_OPEN_MSTEN|SPI_OPEN_MSSEN|SPI_MODE8_ON ,640);
SpiChnOpen(Device,SPI_OPEN_MSSEN|SPI_OPEN_MODE8|SPI_OPEN_ON|SPI_OPEN_CKE_REV ,300);

}

void __ISR(_UART_2_VECTOR, ipl5) UART2Handler(){ 
	int val;
	char tempchar, rdData;
	int intchar;

	// Check if a new char arrived
	if (mU2RXGetIntFlag()) {
		if (U2STAbits.URXDA) {		// wait for new char to arrive
			tempchar = U2RXREG;		// read the char from receive buffer		
//			SpiChnGetC(3);
//			while(!SpiChnTxBuffEmpty(3))	//checks the tx buffer if full wait 
//				{}
//				SpiChnWriteC(3,tempchar);				//loads the char c in to the device selected
			charsave[datacount]=tempchar;
			datacount++;
			}
			if(datacount == 100)
			{
				datacount = 0;
				looped = 1;
			}
		mU2RXClearIntFlag();
	}
	if (mU2TXGetIntFlag()) {
		// If chars are ready to send and TX buffer is not full
		while (txcnt && !U2STAbits.UTXBF) {
		}
		mU2TXClearIntFlag();
	}	

}	


