/**   49LF040B.h

	LPC/49LF040B DRIVER FOR MSP430 DEVICES.

	Defines and stuff.
	Things that start with "_" are supposed to be used anywhere.
	If they start with "__" they are supposed to be used only 
	by the driver code itself.
	
	No warranty and free as in freedom.
*/

/// DEFINE PINS HERE
/// LAD must be on P2
/// Ctl must be on P1

//LAD (P2)
#define __040_LAD0	BIT0
#define __040_LAD1	BIT1
#define __040_LAD2	BIT2
#define __040_LAD3	BIT3
//Control lines (P1)
#define __040_LFRM	BIT4
#define __040_RSET	BIT5
#define __040_CLK	BIT6


///Public function declarations
unsigned int _040_INIT( void );//init driver
unsigned char _040_WB( unsigned long , unsigned char );//Write Byte
unsigned char _040_RB( unsigned long );//Read byte
unsigned char _040_FE( void );//Full erase

///Private declarations
#define __040_TRIS_OUT	1
#define __040_TRIS_IN	0

void __040_TRIS_LAD( unsigned char );
void __040_LAD( unsigned char );
void __040_TICK ( unsigned int );

unsigned char __040_PR( unsigned char, unsigned char );