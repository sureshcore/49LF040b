//Intel's LPC (low pin count) driver
//Public domain, 0% warranty.
//@do_not_noot
#include <msp430.h>
#include <legacymsp430.h>
#include "uart.h"
#include "Intel_LPC.h" //<-- set pins here

//initialize driver
int LPC_Init() {
	//Set TRIS for P1 and P2
	P1DIR |= LPC_CLOCK + LPC_RESET + LPC_INIT + LPC_LFRAME; 
	P2DIR |= LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3;

	//clock low
	LPC_SCLOCK(0);
	
	//LFRAME high
	LPC_SLFRAME(1);
	
	//reset HIGH
	LPC_SRESET(1);

	return 0;
}

//set clock
void LPC_SCLOCK(int a) {
	if (a) {
		P1OUT |= LPC_CLOCK;
	} else {
		P1OUT &= ~LPC_CLOCK;
	}
}

//set LFRAME
void LPC_SLFRAME(int a) {
	if (a) {
		P1OUT |= LPC_LFRAME;
	} else {
		P1OUT &= ~LPC_LFRAME;
	}
}

//set clock
void LPC_SRESET(a) {
	if (a) {
		P1OUT |= LPC_RESET;
	} else {
		P1OUT &= ~LPC_RESET;
	}
}

//set LAD[3:0]
void LPC_SLAD( unsigned char data ) { //complete shit but works. TODO: get this done as it should be done.
	//flush LAD pins:
	P2OUT &= ~( LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3 );
	
	//set them individually
	if( data & BIT0 ) {
		P2OUT |= LPC_LAD0;
	}
	
	if( data & BIT1 ) {
		P2OUT |= LPC_LAD1;
	}
	
	if( data & BIT2 ) {
		P2OUT |= LPC_LAD2;
	}
	
	if( data & BIT3 ) {
		P2OUT |= LPC_LAD3;
	}
}

//LPC Write
int LPC_Write(int data, long address) {
	int i;
	int ret;
	unsigned char add_split[8];
	
	//split address into 8 nibbles
	for( i = 8; i > 0; i--) {
		add_split[i] = address;
		uart_putc( add_split[i] );
		address = address >> 4;
	}
	
	//-1 CK cycle: 
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
		
		
	//CK cycle 1 START
	LPC_SLAD(0x00);
	LPC_SLFRAME(0);
	
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	LPC_SLFRAME(1);
	
	//CK cycle 2 CYCTYPE + DIR
	LPC_SLAD(0b00000110); //read memory
	
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK cycles 3-10: write memory address to read (long address)
	for( i = 0; i < 8; i++ ) {
		LPC_SLAD(add_split[i]);
		LPC_SCLOCK(1);
		LPC_SCLOCK(0);		
	}
	
	//CK cycle 11 DATA
	LPC_SLAD(data);
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK cycle 12 DATA
	LPC_SLAD(data >> 4);
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK cycle 13 TAR0
	LPC_SLAD(0xFF);
	LPC_SCLOCK(1);
	
	P2DIR &= ~(LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3);
	
	LPC_SCLOCK(0);
	
	//CK Cycle 14 TAR1
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK Cycle 15 SYNC
	LPC_SCLOCK(1);
	ret = P2IN;
	LPC_SCLOCK(0);
	
	//CK Cycle 16 TAR0
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK Cycle 17 TAR1
	LPC_SCLOCK(1);
	P2DIR |= LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3;
	LPC_SCLOCK(0);
	
	return ret;
}

//LPC Read
unsigned char LPC_Read(unsigned long address) {
	int i;
	int data = 0;
	int data0 = 0;
	int data1 = 0;
	unsigned char add_split[8];
	
	//reverse address
	address = reverse( address );
	
	//split address into 8 nibbles
	for( i = 8; i > 0; i--) {
		add_split[i] = address;
		address = address >> 4;
	}
	
	//-1 CK cycle: 
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
		
		
	//CK cycle 1 START
	LPC_SLAD(0x00);
	LPC_SLFRAME(0);
	
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	LPC_SLFRAME(1);
	
	//CK cycle 2 CYCTYPE + DIR
	LPC_SLAD(0b00000100); //read memory
	
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK cycles 3-10: write memory address to read (long address)
	for( i = 1; i < 9; i++ ) {
		LPC_SLAD(add_split[i]);
		LPC_SCLOCK(1);
		LPC_SCLOCK(0);		
	}
	
	//CK cycle 11 TAR0
	LPC_SLAD(0xFF);
	LPC_SCLOCK(1);
	
	P2DIR &= ~(LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3);
	
	LPC_SCLOCK(0);
	
	//CK Cycle 12 TAR1
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK Cycle 13 SYNC
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK Cycle 14 DATA0
	LPC_SCLOCK(1);
	data0 = P1IN;
	LPC_SCLOCK(0);
	
	//CK Cycle 15 DATA1
	LPC_SCLOCK(1);
	data1 = P1IN;
	LPC_SCLOCK(0);
	
	//CK Cycle 16 TAR0
	LPC_SCLOCK(1);
	LPC_SCLOCK(0);
	
	//CK Cycle 17 TAR1
	LPC_SCLOCK(1);
	P2DIR |= LPC_LAD0 + LPC_LAD1 + LPC_LAD2 + LPC_LAD3;
	LPC_SCLOCK(0);
	
	//juntamos ambos nibbles (data0 y data1) en data.
	//igual que antes, tiene que haber una manera mejor de hacer esto. MUY TO-DO ok xd.
	
	//nibble LS
	if( data0 & LPC_LAD0 ) {
		data |= BIT0;
	}
	if( data0 & LPC_LAD1 ) {
		data |= BIT1;
	}
	if( data0 & LPC_LAD2 ) {
		data |= BIT2;
	}
	if( data0 & LPC_LAD3 ) {
		data |= BIT3;
	}
	//nibble MS
	if( data1 & LPC_LAD0 ) {
		data |= BIT4;
	}
	if( data1 & LPC_LAD1 ) {
		data |= BIT5;
	}
	if( data1 & LPC_LAD2 ) {
		data |= BIT6;
	}
	if( data1 & LPC_LAD3 ) {
		data |= BIT7;
	}
	
	return data;
}

unsigned long reverse(register unsigned long x) {
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));
}







