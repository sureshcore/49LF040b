//Intel_LPC.h
//define LPC interface pins here
//LAD in port2
#define LPC_LAD0		BIT0
#define LPC_LAD1		BIT1
#define LPC_LAD2		BIT2
#define LPC_LAD3		BIT3
//control pins in port1
#define LPC_LFRAME		BIT5
#define	LPC_RESET		BIT4
#define LPC_INIT		BIT6
#define	LPC_CLOCK		BIT7

int LPC_Init();
int LPC_Write( int, long );
unsigned char LPC_Read( unsigned long );

void LPC_SLFRAME( int );
void LPC_SCLOCK( int );
void LPC_SLAD( unsigned char );
void LPC_SRESET( int );

unsigned long reverse( register unsigned long );