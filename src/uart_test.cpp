#include <util/delay.h>

#include "uart.h"

#include <util/delay.h>

#define PRINT_BAUDRATE 9600
#define PRINT_DELAY_MS 1200

#define PRINT_STR "All OK!"

int main(int argc, char *argv[])
{
	volatile int delay_var;

	uart0_init(UART_BAUD_SELECT(9600, 8000000));

	for(;;) {

		_delay_ms(PRINT_DELAY_MS);

		uart0_puts(PRINT_STR);
	}

	return 0;
}
