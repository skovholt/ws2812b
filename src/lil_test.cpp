#include "uart.h"

int main(void)
{
	uart0_init(UART_BAUD_SELECT(9600, 8000000));

	uart0_puts("All seems fine\n");

	for(;;) {};

	return 0;
}
