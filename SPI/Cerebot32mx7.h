/* 	Cerebot32MX7
**
**	Richard Wall
**	September 30, 2011
**
*/


#ifndef __CEREBOT32MX7__
	#define __CEREBOT32MX7__
	
	#define LEDA	(unsigned int) BIT_2	// Port B
	#define LEDB	(unsigned int) BIT_3	// Port B
	#define LEDC	(unsigned int) BIT_4	// Port B
	#define LEDD	(unsigned int) BIT_6	// Port B
	#define LEDE	(unsigned int) BIT_7	// Port B
	#define LEDF	(unsigned int) BIT_8	// Port B
	#define LEDG	(unsigned int) BIT_9	// Port B
	#define LEDH	(unsigned int) BIT_10	// Port B
	#define SM1		LEDE
	#define SM2		LEDF
	#define SM3		LEDG
	#define SM4		LEDH

	#define RBLEDS	(LEDA | LEDB | LEDC | LEDD | SM1 | SM2 |SM3 | SM4)
	
	#define BTN1	(unsigned int) BIT_6	// Port G
	#define BTN2	(unsigned int) BIT_7	// Port G
	#define BUTTONS	(BTN1 | BTN2}

	#define BTN3	(unsigned int) BIT_13	// Port D

	#define LED1	(unsigned int) BIT_12	// Port G
	#define LED2	(unsigned int) BIT_13	// Port G
	#define LED3	(unsigned int) BIT_14	// Port G
	#define LED4	(unsigned int) BIT_15	// Port G
	#define RGLEDS	(LED1 | LED2 | LED3 | LED4)

	#define SYS_FREQ		(80000000UL)
	#define PB_DIV          8
	#define CORE_TICK_RATE	(SYS_FREQ >> 1)
	#define FPB				(SYS_FREQ / PB_DIV)

// Timer 1
	#define PRESCALE            256
	#define TOGGLES_PER_SEC     1000
	#define T1_TICK             (SYS_FREQ/PB_DIV/PRESCALE/TOGGLES_PER_SEC)


#endif

