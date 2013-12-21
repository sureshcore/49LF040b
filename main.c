#include <msp430.h>
//#include <legacymsp430.h>

#include "49LF040B.h"
#include "uart.h"


//Interrupt UART RX
void uart_rx_isr(unsigned char c) {
	uart_putc(c);
	P1OUT ^= BIT6;		// toggle P1.0 (red led)
}

//main
int main() {
    WDTCTL  = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
  	DCOCTL  = CALDCO_1MHZ;
 
	P1DIR = BIT6;
 
    uart_init();
	
	_040_INIT();
	
	//Activar INT para RX UART
    uart_set_rx_isr_ptr(uart_rx_isr);
    __bis_SR_register(GIE);
	
	//todo ok
	uart_puts( (char *)"WUOLAA\n\r" );
	
	
	while(1) {
		//__delay_cycles(100000);
		//P1OUT ^= BIT6;
    } 
}