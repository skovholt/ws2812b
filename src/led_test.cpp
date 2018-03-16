#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "RGBStream.hpp"

#include "uart.h"
#include "defines.h"

const struct RGBStream::font_desc TEST_FONT_DESC = {\
	.name = {'S', 'a', 'm', 'p', 'l', 'e', '\0'},\
	.height = 8,\
	.def_foreg_color = (struct RGBStream::color_desc) {180, 100, 180},\
	.def_backg_color = (struct RGBStream::color_desc) {60, 120, 60},\
};
	// We've defined a test font, kept in memory (not external but internal)
	// It shall be used as the first test for everything "OK"
	// Forground is defined cyan'ish and background is defined red'ish.

const struct RGBStream::char_desc TEST_CHAR_O_DESC = {\
	.utf8_val = 'O',\
	.width = 8,\
};

const struct RGBStream::char_desc TEST_CHAR_K_DESC = {\
	.utf8_val = 'K',\
	.width = 8,\
};

const char TEST_CHAR_O_FEED[] = {\
	0b01111110,\
	0b10000001,\
	0b10000001,\
	0b10000001,\
	0b10000001,\
	0b10000001,\
	0b10000001,\
	0b01111110,\
};

//	.111111.
//	1......1
//	1......1
//	1......1
//	1......1
//	1......1
//	1......1
//	0111111.

const char TEST_CHAR_K_FEED[] = {\
	0b10000001,\
	0b11000011,\
	0b01100110,\
	0b00100100,\
	0b00111100,\
	0b00111100,\
	0b11111111,\
	0b11111111,\
};

//	11....11
//	11...11.
//	11..11..
//	1111....
//	1111....
//	11..11..
//	11...11.
//	11....11

const unsigned char DANK_CHAR_K_FEED[] = {\
	0b10000001,\
	0b11000011,\
	0b01100110,\
	0b00100100,\
	0b00111100,\
	0b00111100,\
	0b11111111,\
	0b11111111,\
};

void stream_K()
{
	volatile char x = 0;
	char i;
	char j;

	x++; x++; x++; x++; x++;	// Inducing some wait

	PORTD = 0b00000000;
	DDRD = 0b11111111;

	for(char j = 0; j < 8; j++) {
	for(char i = 0; i < 24; i++) {

		PORTD = 0b11111111;

#if ALL_LIT==1
		PORTD = 0b11111111;
#else
		PORTD = DANK_CHAR_K_FEED[j]; // Drop the zeros;
#endif
		

#if QUANTUM_DELAY > 1
		x++;
#endif
#if QUANTUM_DELAY > 0
		x++;
#endif

		PORTD = 0b00000000;	// Drop all

#if INDUCE_DELAY > 0
		x++;
#endif
#if INDUCE_DELAY > 1
		x++;
#endif
#if INDUCE_DELAY > 2
		x++;
#endif
#if INDUCE_DELAY > 3
		x++;
#endif
#if INDUCE_DELAY > 4
		x++;
#endif
#if INDUCE_DELAY > 5
		x++;
#endif
#if INDUCE_DELAY > 6
		x++;
#endif
#if INDUCE_DELAY > 7
		x++;
#endif
#if INDUCE_DELAY > 8
		x++;
#endif
#if INDUCE_DELAY > 9
		x++;
#endif
#if INDUCE_DELAY > 10
		x++;
#endif
#if INDUCE_DELAY > 11
		x++;
#endif
#if INDUCE_DELAY > 12
		x++;
#endif
#if INDUCE_DELAY > 13
		x++;
#endif
#if INDUCE_DELAY > 14
		x++;
#endif
#if INDUCE_DELAY > 15
		x++;
#endif
	}
	}

	return;
}

void timing_test()
{
	volatile char x;

	PORTD = 0b00000000;
	DDRD = 0b11111111;

#if QUICK_START==0
	_delay_ms(100);
#endif

	for(short i = 0; i < TOTAL_BITS; i++) {

		x = 0;

		PORTD = 0b11111111;

		for(x; x < DELAY_STEPS; x++) {
			// Delay loop
		}

#if PIN_TOGGLE==1
		PIND = 0b11111111;
#else
		PORTD = 0b00000000;
#endif

#ifdef REST
		_delay_ms(1 + 2 * REST);
#endif
		
	}
}

int main(int argc, char *argv[])
{
	RGBStream test_rs;
	char font[256];	// We'll hold the test font here
	char *marker;
	RGBStream::font_desc *f_desc;

	marker = font;
	f_desc = (RGBStream::font_desc *) font;

	DDRD = 0b11111111;
	PORTD = 0b11111111;

	uart0_init(UART_BAUD_SELECT(9600, 8000000));

	uart0_puts("Beginning..\n");

//	uart0_puts("Below two lines should match:\n");

//	uart0_puts("OK\n");
//	uart0_putc(((struct RGBStream::font_desc) font)->char;

//	uart0_puts("Streaming..\n");

	memcpy(marker, &TEST_FONT_DESC, sizeof(struct RGBStream::font_desc));
	marker += sizeof(struct RGBStream::font_desc);

	memcpy(marker, &TEST_CHAR_O_DESC, sizeof(struct RGBStream::char_desc));
	marker += sizeof(struct RGBStream::char_desc);

	memcpy(marker, TEST_CHAR_O_FEED, sizeof(TEST_CHAR_O_FEED));
	marker += sizeof(TEST_CHAR_O_FEED);

	memcpy(marker, &TEST_CHAR_K_DESC, sizeof(struct RGBStream::char_desc));
	marker += sizeof(struct RGBStream::char_desc);

	memcpy(marker, TEST_CHAR_K_FEED, sizeof(TEST_CHAR_K_FEED));
	marker += sizeof(TEST_CHAR_O_FEED);

	memset(marker, 0, sizeof(struct RGBStream::char_desc));

	uart0_puts("Testing font ..\n");

	uart0_puts("Font name:\t");
	uart0_puts(f_desc->name); uart0_putc('\n');

	uart0_puts("First char:\t");
	uart0_putc(f_desc->char_desc_ar->utf8_val); uart0_putc('\n');

	uart0_puts("Second char:\t");
	uart0_putc(((struct RGBStream::char_desc *) (((char *) (f_desc->char_desc_ar)) + sizeof(struct RGBStream::char_desc) + 8))->utf8_val); uart0_putc('\n');

	stream_K();

//	timing_test();

//	test_rs.displayString((struct RGBStream::font_desc *) font, "OK");


	uart0_puts("Finished.\n");

	uart0_puts("Waiting for command now. Send any string to stream\n");
	for(;;) {
		char recv_buffer[32];
		char mark;
		if(uart0_available()) {
			mark = 0;

			// Collect the characters
			while(uart0_available()) {
				recv_buffer[mark++] = uart0_getc();
			}
			recv_buffer[mark] = '\0';
			
			// Stream it now
			test_rs.displayString((struct RGBStream::font_desc *) font, recv_buffer);
		}
	};

	return 0;
}
