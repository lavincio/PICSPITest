/*  PIC SPI Interrupt Test
	This program takes input from a serial terminal (interrupt based UART)
	and output the data to the display screen via SPI interrupt.
	Once the data is overflow from row 1, it switches to row 2 then back to row 1 etc.
*/

#include <plib.h>
#include <stdio.h>
#include <proc/p32mx795f512l.h>
#include "conU1.h"
#include "Cerebot32mx7.h"
#include "config_bits(4).h"

typedef struct DisBuffer 														//display buffer structure 16 chars by 4 lines
{
char line [16][1];
} buffer;

#define CHAR_ARR_SIZE 20

// UART 1 is on connector JE of the Cerebot MX7ck board
// timing and baud rate settings 

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
char 	tempchar = 0;
volatile int x=0,i=0;		// x: counting the current row number, i: counting the number of bits

void IniBuff(buffer *Buff);  													//initialize display buffer
void DisplayBuff(int Device, buffer *Buff,int line); 							//display the selected buffer line to current cursor location
void LoadBuff (char *string, BYTE row, buffer *Buff,int slen);					//loads strings into the display buffer
void Clrscreen(int Device);														//clears the pmod CLS 
void MoveRow(int Device,int row);												//home the cursor to row 0 or row 1 
int spi();															//spi initialization (SPI4 or spi4) 

//Device settings
//1 = SPI1 port listed in the schematic pmod header JD since this is easier using peripheral library.

//SPI Interrupt to detect data read in

void __ISR(_SPI_1_VECTOR, ipl7) Spi1Handler()
    {
mPORTGToggleBits(BIT_14);
	static BYTE data = 0;
	if(SpiChnGetRxIntFlag(1)) {


		SpiChnPutC (1, 'a');
		data=SpiChnGetC(1);// get the received data


//data=getU(1);
		putU2(data);
mPORTGToggleBits(BIT_12);
        SpiChnClrRxIntFlag(1);   
 }

}

void __ISR(_SPI_3_VECTOR, ipl6) Spi3Handler()
{
	static BYTE data = 0;
    if(SpiChnGetTxIntFlag(3)){
//mPORTGToggleBits(BIT_12);
 		//SpiChnPutC (3, data);
        SpiChnClrTxIntFlag(3);
	}
	if(SpiChnGetRxIntFlag(3)) {
		data=SpiChnGetC (3);
		//SpiChnPutC (3, data);
		//putU2(data);
mPORTGToggleBits(BIT_13);
        SpiChnClrRxIntFlag(3);
	}   
}


//Main program
int main(void)
{	

	buffer Dbuff;						//create the display buffer
	mPORTGSetPinsDigitalOut(BIT_12|BIT_13|BIT_14|BIT_15);


	// Enable multi-vector interrupts.
	INTEnableSystemMultiVectoredInt();
	initU1();

	spi();
	while(1);							// wait for interrupt
}


int initU1(void)
{
	unsigned int BRG;

	BRG = (unsigned short)(( (float)FPB / ( (float)4 * (float) BRATE ) ) - (float)0.5);
	//OpenUART1( UART_EN, 
	//UART_RX_ENABLE | UART_TX_ENABLE | UART_BRGH_FOUR | UART_INT_TX_BUF_EMPTY | UART_INT_RX_CHAR, BRG );
	OpenUART2( UART_EN, 
	UART_RX_ENABLE | UART_TX_ENABLE | UART_BRGH_FOUR | UART_INT_TX_BUF_EMPTY | UART_INT_RX_CHAR, BRG );

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
//open CKE in SPI mode 0, baud rate set to 40Mhz/1024
// Peripheral Library function replaces text method for setting SPI

int spi()
{
//SpiChnOpen(Device,SPI_OPEN_MSTEN|SPI_OPEN_MSSEN|SPI_OPEN_MODE8|SPI_OPEN_TBE_SR_EMPTY|SPI_OPEN_ON|SPI_OPEN_CKE_REV ,640);
	SpiChnOpen(3, SPICON_MSTEN|SPICON_FRMEN|SPICON_SMP|SPICON_ON, 20);
   	ConfigIntSPI3(SPI_TX_INT_EN |SPI_INT_PRI_6|SPI_INT_SUB_PRI_2);
    EnableIntSPI3;
	SpiChnOpen(1, SPICON_FRMEN|SPICON_FRMSYNC|SPICON_SMP|SPICON_ON, 20);
	ConfigIntSPI1(SPI_RX_INT_EN |SPI_INT_PRI_7|SPI_INT_SUB_PRI_1);
    EnableIntSPI1;
}

//********************************************************************************************************************************//
//Basic put and get functions

int putU(int Device, int c)
{
if((Device == 3) || (Device == 1))
	{while(!SpiChnTxBuffEmpty(Device))	//checks the tx buffer if full wait 
		{}
	SpiChnWriteC(Device,c);				//loads the char c in to the device selected
	}
else
	{return c;}
}

//get a char from the device
char getU( int Device)
{
	int temp =0;
	if((Device == 3) || (Device == 1))	{
		temp = SpiChnReadC(Device); //read data from the recieve buffer ( i didn't check this just thought you would like to see the function)
		return temp;}
	else
		{return 0xff;}
}

// send a null terminated string to the choosen Device (wrote it didn't need it so did check this if it works)
int putsU( int Device, const char *s)
{
	if((Device == 3) || (Device == 1)){
		while( *s)			// loop until *s == '\0' end of string
		putU(Device,*s++);
		}
	else
		{return 0;}			// loop until *s == '\0' end of string

return 1;
} 



void __ISR(_UART_2_VECTOR, ipl5) UART2Handler(){ 
	int val;
	char tempchar, rdData;
	int intchar;
	buffer Dbuff;						//create the display buffer
	char c1[] = " ";	//stings to load into buffer
	char collect[17];					//create char string to display all at once

	// Check if a new char arrived
	if (mU2RXGetIntFlag()) {
		if (U2STAbits.URXDA) {		// wait for new char to arrive
			tempchar = U2RXREG;		// read the char from receive buffer		
			intchar = tempchar - '0';//convert from char to int
mPORTGToggleBits(BIT_15);			
			SpiChnPutC(3, tempchar);// send data on the master channel
		//	rdData=getU(3);
//putU2(rdData);
		}
		mU2RXClearIntFlag();
	}
	if (mU2TXGetIntFlag()) {

		// If chars are ready to send and TX buffer is not full
		while (txcnt && !U2STAbits.UTXBF) {
			// *** transmit next buffered char
			//tempchar = U2TXREG;	
			//putU2(tempchar);
		}
		mU2TXClearIntFlag();
	}	

}	



int putU1( int c)
{	
	U1TXREG = c;	
	return c;
}

char getU1( void)
{
	char temp;

	temp = tempchar;
	tempchar = 0;
	return temp;
}

int putU2( int c)
{	
	U2TXREG = c;	
	return c;
}

char getU2( void)
{
	char temp;

	temp = tempchar;
	tempchar = 0;
	return temp;
}


    
