#include <string.h>

#include "RGBStream.hpp"

#include "uart.h"

struct just_nohin {
	int nothing;
	char nothing_burger;
};

#define NOHIN (struct just_nohin) {\
	.nothing = 0,\
	.nothing_burger = 1\
}

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

int main(int argc, char *argv[])
{
	RGBStream test_rs;
	char font[512];	// We'll hold the test font here
	char *marker;

	marker = font;

	uart0_init(UART_BAUD_SELECT(9600, 8000000));

	uart0_puts("Beginning..\n");

//	memcpy(marker, &NOHIN, sizeof(struct just_nohin));
//	marker += sizeof(struct just_nohin);

//	uart0_puts("Below two lines should match:\n");

//	uart0_puts("OK\n");
//	uart0_putc(((struct RGBStream::font_desc) font)->char

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

	test_rs.displayString((struct RGBStream::font_desc *) font, "OK");

	uart0_puts("Finished.\n");

	for(;;) {};

	return 0;
}
