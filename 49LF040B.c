//! 49LF040B.c

/** 49LF040B (or similar) LPC DRIVER FOR MSP430 DEVICES.
//  REFER TO 49LF040B TO SET PINS AND REFERENCE.
//  -ABSOLUTELY NO WARRANTY-
//  and free as in freedom.
*/

#include <msp430.h>
#include <legacymsp430.h>

#include "49LF040B.h"

///Init Function
unsigned int _040_INIT( void ) {
	//set TRIS and PxDIR
	P1DIR |= ( __040_LFRM + __040_RSET + __040_CLK ); //DIR out
	__040_TRIS_LAD( __040_TRIS_IN );		//tris LAD to IN
	
	/*P2DIR &= ~( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //DIR in
	P2REN |=  ( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //REN on
	P2OUT |=  ( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //REN to VCC*/
	
	//unreset and set lines as default
	P1DIR |= __040_LFRM;
	__040_TICK( 0 );
	__delay_cycles(10);
	P1DIR &= ~__040_RSET;

	return 0;
}

///Write Byte
unsigned char _040_WB( unsigned long address, unsigned char data ) {
	//tbd
	
	return 0;
}

///Read byte
unsigned char _040_RB( unsigned long address ) {
	unsigned char data[3] = {0x00, 0x00, 0x00}; //<f returns this
	int i;
	int n;

	//split address in 8 nibbles
	//(not actually nibbles but it'll do.)
	unsigned char nibbles[8];
	for( i = 7; i >= 0; i-- ) {
		nibbles[i] = address;
		address = address >> 4;
	}
	
	//Start of the cycle:
		//LFRAM DOWN
		P1DIR &= ~__040_LFRM;
		
		//tris LAD out
		__040_TRIS_LAD( __040_TRIS_OUT );
		
		//set lad to 0x0
		__040_LAD( 0x00 );
		
		//CLK UP < LATCH HERE
		__040_TICK( 1 );
	
		//LFRAME up again, cycle starts
		P1DIR |= __040_LFRM;
		
		//CLK DOWN, cycle done
		__040_TICK( 0 );

	//Tick 2, send MREAD command
		//SET LAD
		__040_LAD( 0x04 );
		
		//tick tack, cycle done
		__040_TICK( 1 );
		__040_TICK( 0 );
	
	//Ticks 3-10 (incl.), Send Address
		for( n = 0; n < 8; n++ ) {
			__040_LAD( nibbles[n] );
			__040_TICK( 1 );
			__040_TICK( 0 );
		}
	
	//Tick 11, TAR0
		//LAD 0xF
		__040_LAD( 0xF );
		__040_TICK( 1 );
		__040_TICK( 0 );
		
	//Tick 12, TAR1
		//change TRIS
		__040_TRIS_LAD( __040_TRIS_IN );
		__040_TICK( 1 );
		__040_TICK( 0 );
		
	//Tick 13, Sync
		//Do nothing. 0x0 should be on LAD
		__040_TICK( 1 );
		__040_TICK( 0 );
		
	//Tick 14, DATA!!
		//get dem bits nigga
		__040_TICK( 1 );
		data[0] = P2IN;
		__040_TICK( 0 );
		
	//Tick 15, DATA(cont.)!!
		//get dem bits nigga
		__040_TICK( 1 );
		data[1] = P2IN;
		__040_TICK( 0 );
		
	//Tick 16, TAR back
		__040_TRIS_LAD( __040_TRIS_OUT );
		
	data[2] = __040_PR( data [0], data[1] );
	
	return data[2];
}

///Full erase
unsigned char _040_FE( void ) {
	
	//tbd
	
	return 1;
}

///Rest of functions to be used by driver itself:
///Set LAD[3:0] pins
void __040_LAD( unsigned char __a ) {
	//clear LAD pins
	P2OUT &= ~( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 );
	
	//set LAD pins
	if( BIT0 & __a ) {
		P2OUT |= __040_LAD0;
	}
	if( BIT1 & __a ) {
		P2OUT |= __040_LAD1;
	}
	if( BIT2 & __a ) {
		P2OUT |= __040_LAD2;
	}
	if( BIT2 & __a ) {
		P2OUT |= __040_LAD3;
	}
}

///Set TRIS to the pins
void __040_TRIS_LAD( unsigned char __a ) {
	if( __a ) { //out		
		P2DIR |=  ( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //DIR OUT
		P2REN |=  ( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //REN OFF
		P2OUT &= ~( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //Clear LAD
	} else {	//in
		P2DIR &= ~( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //DIR in
		P2REN &= ~( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //REN on
		P2OUT |=  ( __040_LAD0 + __040_LAD1 + __040_LAD2 + __040_LAD3 ); //REN to VCC
	}
}

///tick clock
void __040_TICK ( unsigned int __a ) {
	if ( __a ) {
		P1OUT |= __040_CLK;
	} else {
		P1OUT &= ~__040_CLK;  
	}
}

/// Process read nibbles
unsigned char __040_PR( unsigned char __a, unsigned char __b ) { //a=LSB, b=MSB
	unsigned char __c = 0;
	
	//lsb
	if( __040_LAD0 & __a ) {
		__c |= BIT0;
	}
	if( __040_LAD1 & __a ) {
		__c |= BIT1;
	}
	if( __040_LAD2 & __a ) {
		__c |= BIT2;
	}
	if( __040_LAD3 & __a ) {
		__c |= BIT3;
	}
	
	//msb
	if( __040_LAD0 & __b ) {
		__c |= BIT4;
	}
	if( __040_LAD1 & __b ) {
		__c |= BIT5;
	}
	if( __040_LAD2 & __b ) {
		__c |= BIT6;
	}
	if( __040_LAD3 & __b ) {
		__c |= BIT7;
	}
	
	return __c;
}



