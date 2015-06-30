/*
** CONU2.h
** console I/O library for Explorer16 board
** v 2.0 7/18/07 LDJ 
** Modified for Cerebot 32MX7 
** Richard Wall
** October 2, 2011
**
*/

#ifndef __CONU1

	#define __CONU1
	#define _UART4 

// I/O definitions for the Explorer16
// No hardware handshaking supported
#define CTS 	_RF12   // Cleart To Send, input, HW handshake
#define RTS     _RF13   // Request To Send, output, HW handshake
#define BACKSPACE 0x8   // ASCII backspace character code            


// send a character to the serial port
int putU(int Device, int c);

// wait for a new character to arrive to the serial port
char getU(int Device);

// send a null terminated string to the serial port
int putsU( int Device, const char *s);

// receive a null terminated string in a buffer of len char
//char * getsn( char *s, int n);

// useful macros
#define clrscr() puts( "\x1b[2J") 
#define home()   puts( "\x1b[1,1H") 

/*	Printf support
** Richard Wall
** October 2, 2011
*/
	void _mon_putc(char c);  // Used by system only

#endif

void DelayMs( WORD tmsDelay );
void DelayUs( WORD tusDelay );

/* ------------------------------------------------------------ */
/***    DelayMs
**
**	Synopsis:
**		DelayMs(tmsDelay)
**
**	Parameters:
**		tmsDelay - the number of milliseconds you want to delay
**
**	Return Values:
**      none
**
**	Errors:
**		none
**
**	Description:
**		This procedure delays program execution for the specified number
**      of miliseconds.
*/
void DelayMs( WORD tmsDelay )
{	
	while ( 0 < tmsDelay ) {
		DelayUs(1000);
		tmsDelay--;
	}
}

/* ------------------------------------------------------------ */
/***    DelayUs
**
**	Synopsis:
**		DelayUs(tusDelay)
**
**	Parameters:
**		tusDelay - the amount of time you wish to delay in microseconds
**
**	Return Values:
**      none
**
**	Errors:
**		none
**
**	Description:
**		This procedure delays program execution for the specified number
**      of microseconds. Please note that the minimal delay supported by
**		this routine is 3 microseconds.
**		
**	Note:
**		This routine is written with the assumption that the
**		system clock is 64 MHz.
*/
void DelayUs( WORD tusDelay )
{
	tusDelay -= 2;
	
	while ( 0 < tusDelay )
    {
        tusDelay--;
    }   // end while
}


#define EnableIntSPI3	(mSPI3ClearAllIntFlags(), mSPI3RXIntEnable(1), mSPI3TXIntEnable(1) )
#define	ConfigIntSPI3(config) (mSPI3IntDisable(), mSPI3ClearAllIntFlags(), mSPI3SetIntPriority((config)&0x7), \
				mSPI3SetIntSubPriority(((config)&0x18)>>3), mSPI3SetIntEnable(((config)&0xe0)>>5))

#define EnableIntSPI4	(mSPI4ClearAllIntFlags(), mSPI4RXIntEnable(1), mSPI4TXIntEnable(1) )
#define	ConfigIntSPI4(config) (mSPI4IntDisable(), mSPI4ClearAllIntFlags(), mSPI4SetIntPriority((config)&0x7), \
				mSPI4SetIntSubPriority(((config)&0x18)>>3), mSPI4SetIntEnable(((config)&0xe0)>>5))

/*********************************************************************
 * SPI 3 Interrupt Control Functions
 ********************************************************************/
/*********************************************************************
 * SPI 3 Error
 ********************************************************************/
#define mSPI3EClearIntFlag()                (IFS0CLR = (1 << 26))
#define mSPI3EGetIntFlag()                  (IFS0bits.SPI3EIF)
#define mSPI3EGetIntEnable()                (IEC0bits.SPI3EIE)
#define mSPI3EIntEnable(enable)             (IEC0CLR = (1 << 26), IEC0SET = (enable << 26))
/*********************************************************************
 * SPI 3 Transfer
 ********************************************************************/
#define mSPI3TXClearIntFlag()               (IFS0CLR = (1 << 28))
#define mSPI3TXGetIntFlag()                 (IFS0bits.SPI3TXIF)
#define mSPI3TXGetIntEnable()               (IEC0bits.SPI3TXIE)
#define mSPI3TXIntEnable(enable)            (IEC0CLR = (1 << 28), IEC0SET = (enable << 28))
/*********************************************************************
 * SPI 3 Receive
 ********************************************************************/
#define mSPI3RXClearIntFlag()               (IFS0CLR = (1 << 27))
#define mSPI3RXGetIntFlag()                 (IFS0bits.SPI3RXIF)
#define mSPI3RXGetIntEnable()               (IEC0bits.SPI3RXIE)
#define mSPI3RXIntEnable(enable)            (IEC0CLR = (1 << 27), IEC0SET = (enable << 27))
/*********************************************************************
 * SPI 3 Vector
 ********************************************************************/
#define mSPI3ClearAllIntFlags()              ( IFS0CLR = (7 << 26) )
#define mSPI3IntDisable()                    ( IEC0CLR = (7 << 26) )
#define mSPI3SetIntEnable(flags)			 ( IEC0SET = (((flags)&0x7) << _IEC0_SPI3EIE_POSITION))
#define mSPI3SetIntPriority(priority)        (IPC6CLR = (7 << 2), IPC6SET = (priority << 2))
#define mSPI3GetIntPriority()                (IPC6bits.SPI3IP)
#define mSPI3SetIntSubPriority(subPriority)  (IPC6CLR = (3), IPC6SET = (subPriority))
#define mSPI3GetIntSubPriority()             (IPC6bits.SPI3IS) 

/*********************************************************************
 * SPI 4 Interrupt Control Functions
 ********************************************************************/
/*********************************************************************
 * SPI 4 Error
 ********************************************************************/
#define mSPI4EClearIntFlag()                (IFS1CLR = (1 << 8))
#define mSPI4EGetIntFlag()                  (IFS1bits.SPI4EIF)
#define mSPI4EGetIntEnable()                (IEC1bits.SPI4EIE)
#define mSPI4EIntEnable(enable)             (IEC1CLR = (1 << 8), IEC1SET = (enable << 8))
/*********************************************************************
 * SPI 4 Transfer
 ********************************************************************/
#define mSPI4TXClearIntFlag()               (IFS1CLR = (1 << 10))
#define mSPI4TXGetIntFlag()                 (IFS1bits.SPI4TXIF)
#define mSPI4TXGetIntEnable()               (IEC1bits.SPI4TXIE)
#define mSPI4TXIntEnable(enable)            (IEC1CLR = (1 << 10), IEC1SET = (enable << 10))
/*********************************************************************
 * SPI 4 Receive
 ********************************************************************/
#define mSPI4RXClearIntFlag()               (IFS1CLR = (1 << 9))
#define mSPI4RXGetIntFlag()                 (IFS1bits.SPI4RXIF)
#define mSPI4RXGetIntEnable()               (IEC1bits.SPI4RXIE)
#define mSPI4RXIntEnable(enable)            (IEC1CLR = (1 << 9), IEC1SET = (enable << 9))
/*********************************************************************
 * SPI 4 Vector
 ********************************************************************/
#define mSPI4ClearAllIntFlags()              ( IFS1CLR = (7 << 8) )
#define mSPI4IntDisable()                    ( IEC1CLR = (7 << 8) )
#define mSPI4SetIntEnable(flags)			 ( IEC1SET = (((flags)&0x7) << _IEC1_SPI4EIE_POSITION))
#define mSPI4SetIntPriority(priority)        ( IPC8CLR = (7 << 2), IPC8SET = (priority << 2))
#define mSPI4GetIntPriority()                (IPC8bits.SPI4IP)
#define mSPI4SetIntSubPriority(subPriority)  ( IPC8CLR = (3), IPC8SET = (subPriority))
#define mSPI4GetIntSubPriority()             (IPC8bits.SPI4IS) 