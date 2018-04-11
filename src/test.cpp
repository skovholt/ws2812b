#include <avr/io.h>
#include <avr/delay.h>
#include "uart.h"

#include "TextStream.hpp"

int main(void)
{
	bool rv_b;
	TextStream tx;

	DDRD = 0b100;
	PORTD = 0b100;

	uart0_init(UART_BAUD_SELECT(9600 * 3, 8000000));

	uart0_puts("Beginning..\n");

	_delay_ms(5);

	rv_b = tx.init(0);
	if(!rv_b) {
		uart_puts("Initialisation of SD card failed! Aborting..\n");

		PIND = 0b100;
		for(;;){};	
	} else {
		PIND = 0b100;
		uart_puts("FatFS initialised\n");
	}

	if(tx.demofont_test()) {
		uart0_puts("demofont test passed\n");
	}
	if(tx.democolor_test()) {
		uart0_puts("democolor test passed\n");
	}

	uart0_puts("Tests over.\n");

	for(;;) {}

	return 0;
}


