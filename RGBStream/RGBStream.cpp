#include "RGBStream.hpp"

#define BITS_PER_BYTE 8

// RGBStream functions need to be given a line to
// set the LED array ablaze!

struct char_desc * RGBStream::find_char_desc_in_font(	int utf8_char,
							char feed_is_short,
							struct font_desc *font)
{
	// We loop through a font structure and return a pointer to the
	// desired character description

	for(	struct char_desc *pchar_desc = font->char_desc_ar; 
		pchar_desc->utf8_val != 0;
		pchar_desc = 	((char *) pchar_desc) + (feed_is_short ? \
				pchar_desc->width * 2 : pchar_desc->width) \
				+ sizeof(struct char_desc))
	{
		// The for loop functions so: We start at the char_desc array,
		// We increment to the next array by skipping the struct 
		// char_desc size + the width of the char feed array at the end 
		// of the struct. We continue until we hit the empty last 
		// structure.

		if(pchar_desc->utf8_val == utf8_char) return pchar_desc;
	}

	return NULL;
}

char RGBStream::stream_char_feed(	struct char_desc *feed_char,
					char feed_is_short,
					struct color_desc foreg_col,
					struct color_desc backg_col)
{
	// Stream a char feed to the PORTs

	for(char w = 0; w < feed_char->width; w++) {
		// We go through each feed byte(s) (char or short)
		// Num of feed bytes = width

		short int final_feed;

		if(feed_is_short) {
			final_feed = feed_char->feed[w];
		} else {
			final_feed = ((char *) feed_char->feed)[w];
		}

	for(char i = 0; i < 3; i++) {	// Loop through each color byte
		char fcol_byte, bcol_byte;	// Store color for loop

		// One color byte per loop
		switch(i) {

		case 0:
			fcol_byte = foreg_col.green;
			bcol_byte = backg_col.green;
			break;
		case 1:
			fcol_byte = foreg_col.red;
			bcol_byte = backg_col.red;
			break;
		case 2:
			fcol_byte = foreg_col.blue;
			bcol_byte = backg_col.blue;
			break;
		case default:
			break;
		}

		// Loop through all 8 bits of a color
		for(char j = 0x80; j; j >>= 1) {
		// Set the final feed acc. to foreg and backg bits
		// The final feed should be equal to the ~ of the
		// desired signal bit
			if(fcol_byte && j) {
				if(bcol_byte && j) {
				// Required signal is all ones
				// remember final_feed is ~ of the 
				// desired signal
					final_feed = 0x0;
				} else {
				// Required signal is foreg = ones
				// and background = zeros
				// final_feed is ~ of required
					final_feed ~= final_feed;
				}
			} else {
				if(bcol_byte && j) {
				// Required signal is foreg = zeroes
				// and backg = ones
				// final_feed is ~ of required
				// which is already the case
				} else {
				// Required signal is all zeroes
				// Since final_feed is ~ of req.
				// we set it to all ones
					final_feed = 0xFFFF;
				}
			}

			PORTD = 0b11111111;	// The initial high
			if(feed_is_short)	// Set PORT-C too
				PORTC = 0b01111111;

			for(volatile char x; x < TUNE_VAR_1; x++) {			*** TUNE_VAR not yet provided ***
				// Nothing! Just pause
			}

			PIND = ((char *) &final_feed)[0];// Toggle off
			if(feed_is_short)	// Toggle PORTC too
				PINC = ((char *) &final_feed)[1];

			for(volatile char x; x < TUNE_VAR_2; x++) {
				// Just pause
			}

			final_feed ~= final_feed;

			PIND = ((char *) &final_feed)[0];// Toggle off
			if(feed_is_short)	// Toggle PORTC too
				PINC = ((char *) &final_feed)[1];
		}
	}
	}

	return 0;
}

char RGBStream::displayStringWithColor(	struct font_desc *font,
					char *string,
					struct color_desc foreg_col,
					struct color_desc backg_col)
{
	// Displays the whole thing
	// Good idea to collect all the char_desc ptrs and store
	// them on the stack beforehand because we can't exceed
	// the reset time of the signal and searching after the
	// signal has begun would contribute to that delay.

	int utf8_char;
	char feed_is_short, rv = 0;
	struct char_desc *pcd;

	if(font->height > 8) {
		feed_is_short = 1;
	} else {
		feed_is_short = 0;
	}

	for(char i = 0; string[i] != '\0';) {
		if(string[i] < 0) {	// More bytes in utf8 char
			if(string[i + 1] < 0) {	// Even more bytes
				memcpy(	((char *) &utf8_char) + 1,
					string + i, 3);	// 3 bytes
				i += 3;	// Inc. to end of utf8 ch.
			} else {
				memcpy(	((char *) &utf8_char) + 2,
					string + i, 2);	// 2 bytes
				i += 2; // Inc. to end of utf8 ch.
			}
		} else {
			utf8_char = string[i];
			i++;
		}

		// Get the font description
		pcd = find_char_desc_in_font(	utf8_char,
						feed_is_short,
						font);

		if(!pcd) rv = 1;

		stream_char_feed(	pcd,
					feed_is_short,
					foreg_col,
					backg_col);
	}

	return rv;
}

char RGBStream::displayString(	struct font_desc *font,
				char *string)
{
	return displayStringWithColor(	font,
					string,
					font->def_foreg_color,
					font->def_backg_color);
}

char RGBStream::RGBStream() {}
char RGBStream::~RGBStream() {}