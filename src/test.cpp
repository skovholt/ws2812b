#define TXTSTRM_TEST
#define RGBSTRM_TEST

#include "RGBStream.h"

#define TEST_FONT_DESC (struct TextStream::font_desc) {\
	.height = 8,\
	.def_foreg_color = (struct TextStream::color_desc) {180, 100, 180},\
	.def_backg_color = (struct TextStream::color_desc) {60, 120, 60},\
};
	// We've defined a test font, kept in memory (not external but internal)
	// It shall be used as the first test for everything "OK"
	// Forground is defined cyan'ish and background is defined red'ish.

#define TEST_CHAR_O_DESC (struct TextStream::font_desc) {\
	.utf8_val = 'O',\
	.width = 8,\
}

#define TEST_CHAR_K_DESC (struct TextStream::font_desc) {\
	.utf8_val = 'K',\
	.width = 8,\
}

#define TEST_CHAR_O_FEED (char[]) {\
	01111110,\
	10000001,\
	10000001,\
	10000001,\
	10000001,\
	10000001,\
	10000001,\
	01111110,\
}

//	.111111.
//	1......1
//	1......1
//	1......1
//	1......1
//	1......1
//	1......1
//	0111111.

#define TEST_CHAR_K_FEED (char[]) {\
	10000001,\
	11000011,\
	01100110,\
	00100100,\
	00111100,\
	00111100,\
	11111111,\
	11111111,\
}

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
	TextStream test_ts;
	char font[512];	// We'll hold the test font here

	memcpy(font, &TEST_FONT_DESC, sizeof(struct TextStream::font_desc));
	memcpy(font, &TEST_CHAR_O_DESC, sizeof(struct TextStream::char_desc));
	memcpy(font, &TEST_CHAR_O_FEED, sizeof(TEST_CHAR_O_FEED));
	memcpy(font, &TEST_CHAR_K_DESC, sizeof(struct TextStream::char_desc));
	memcpy(font, &TEST_CHAR_K_FEED, sizeof(TEST_CHAR_K_FEED));

	if(!test_ts.init()) {
		// Failed initialisation
	}

	test_ts.displayString((struct TextStream::font_desc *) test_ts, "OK");

	return 0;
}
