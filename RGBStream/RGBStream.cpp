#include <avr/io.h>
#include <string.h>

#include "RGBStream.hpp"

#ifdef DEBUG
#if DEBUG == 1
#include "uart.h"
#warning "DEBUG_PRINT is on. Debug info will be printed through UART for the class RGBStream"
#define DEBUG_PRINT(M) uart0_puts(M)
#define DEBUG_PRINT_CHAR(M) uart0_putc(M)
#else
#warning "DEBUG_PRINT is off. No debug info will be printed for the RGBStream class"
#define DEBUG_PRINT(...) do{} while(0)
#define DEBUG_PRINT_CHAR(...) do{} while(0)
#endif
#else
#warning "DEBUG_PRINT is off. No debug info will be printed for the RGBStream class"
#define DEBUG_PRINT(...) do{} while(0)
#define DEBUG_PRINT_CHAR(...) do{} while(0)
#endif

#define BITS_PER_BYTE 8

#ifdef RGBSTRM_8MHZ_TUNING_SAFE
#define TUNE_VAR_0 1
#define TUNE_VAR_1 6
#elif RGBSTRM_8MHZ_TUNING
#define TUNE_VAR_0 2
#define TUNE_VAR_1 5
#else
#ifdef F_CPU
#if F_CPU == 8000000
#define TUNE_VAR_0 1
#define TUNE_VAR_1 6
#else // F_CPU == 8000000
#error "Either define RGBSTREAM_8MHZ_TUNING or RGBSTREAM_8MHZ_TUNING_SAFE or FCPU == 8000000. Check source if you want a new definition."
#endif // FCPU == 8000000
#else // F_CPU
#error "Either define RGBSTREAM_8MHZ_TUNING or RGBSTREAM_8MHZ_TUNING_SAFE or FCPU == 8000000. Check source if you want a new definition."
#endif // F_CPU
#endif // The whole thing

// RGBStream functions need to be given a line to
// set the LED array ablaze!

struct RGBStream::char_desc * RGBStream::find_char_desc_in_font(	
							int utf8_char,
							char feed_is_short,
							struct font_desc *font)
{
	/* This code portion is critical and may be a cause of breakage. If the 
	 * characters list is too long, this function may hold the signal low
	 * for too long causing the reset timeout of the LED strings thus
	 * erasing the written part and rewriting the string from mid-way.
	 * Watchout here if you encounter anything such, though I think there
	 * is little you can do without modifying the other parts of the code
	 * too because not much to optimise here.
	 */

	// We loop through a font structure and return a pointer to the
	// desired character description
	// Two different kinds of loop depending upon the size of each feed,
	// indicated by feed_is_short.

	if(feed_is_short) {

	for(	struct char_desc *pchar_desc = font->char_desc_ar; 
		pchar_desc->utf8_val != 0;
		pchar_desc = 	(struct char_desc *) (((char *) pchar_desc) +\
				(pchar_desc->width * 2) \
				+ sizeof(struct char_desc)))
	{
		// The for loop functions so: We start at the char_desc array,
		// We increment to the next array by skipping the struct 
		// char_desc size + the width of the char feed array at the end 
		// of the struct. We continue until we hit the empty last 
		// structure.

		DEBUG_PRINT_CHAR('\t');
		DEBUG_PRINT_CHAR((char) (pchar_desc->utf8_val));
		DEBUG_PRINT_CHAR('\n');

		if(pchar_desc->utf8_val == utf8_char) return pchar_desc;
	}

	} else {

	for(	struct char_desc *pchar_desc = font->char_desc_ar; 
		pchar_desc->utf8_val != 0;
		pchar_desc = 	(struct char_desc *) (((char *) pchar_desc) +\
				(pchar_desc->width) \
				+ sizeof(struct char_desc)))
	{
		// The for loop functions so: We start at the char_desc array,
		// We increment to the next array by skipping the struct 
		// char_desc size + the width of the char feed array at the end 
		// of the struct. We continue until we hit the empty last 
		// structure.

		DEBUG_PRINT_CHAR('\t');
		DEBUG_PRINT_CHAR((char) (pchar_desc->utf8_val));
		DEBUG_PRINT_CHAR('\n');

		if(pchar_desc->utf8_val == utf8_char) return pchar_desc;
	}

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

			DEBUG_PRINT_CHAR('\n');
			DEBUG_PRINT("Iter");
			DEBUG_PRINT_CHAR(48 + w);
			DEBUG_PRINT_CHAR('\t');

		if(feed_is_short) {
			final_feed = feed_char->feed[w];
		} else {
			final_feed = ((char *) feed_char->feed)[w];
		}

			DEBUG_PRINT_CHAR('\n');
			DEBUG_PRINT_CHAR(((char *) &final_feed)[0]);
			DEBUG_PRINT_CHAR('\t');

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
		default:
			break;
		}

		// Loop through all 8 bits of a color
		for(unsigned char j = 0x80; j; j >>= 1) {
		// Set the final feed acc. to foreg and backg bits
		// The final feed should be equal to the ~ of the
		// desired signal bit

			DEBUG_PRINT_CHAR('|');

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
					final_feed = ~final_feed;
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

//	These lines were added for some debugging
//			volatile char y;
//			if(feed_is_short)
//				y = 1;

// Foregoing the loop and advancing directly for any more delay is 
// too much delay
//			for(volatile char x; x < TUNE_VAR_0; x++) {
//				// Nothing! Just pause
//			}

			PIND = ((char *) &final_feed)[0];// Toggle off
			if(feed_is_short)	// Toggle PORTC too
				PINC = ((char *) &final_feed)[1];
// Was for debugging		PINC = ((char *) &final_feed)[0];

			for(volatile char x; x < TUNE_VAR_1; x++) {
				// Just pause
			}

			final_feed = ~final_feed;

//			DEBUG_PRINT_CHAR(((char *) &final_feed)[0]);

			PIND = ((char *) &final_feed)[0];// Toggle off
			if(feed_is_short)	// Toggle PORTC too
				PINC = ((char *) &final_feed)[1];
// Was for debugging		PINC = ((char *) &final_feed)[0];
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

	DEBUG_PRINT("Showing:");

	// Set ports as output and set them low 
	// (should be low already though)
	DDRD = 0b11111111;
	PORTD = 0b00000000;
	if(feed_is_short) {
		DDRC = 0b01111111;
		PORTC = 0b00000000;
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

		DEBUG_PRINT_CHAR((char) utf8_char);
		DEBUG_PRINT_CHAR('\n');

		DEBUG_PRINT("Searching..\n");

		// Get the font description
		pcd = find_char_desc_in_font(	utf8_char,
						feed_is_short,
						font);

		if(!pcd) {
			DEBUG_PRINT("char_desc not found\n"); rv = 1;
			return rv;
		} else {
			DEBUG_PRINT("Found ");
			DEBUG_PRINT_CHAR((char) (pcd->utf8_val));
			DEBUG_PRINT_CHAR('\n');
		}

		DEBUG_PRINT("Streaming..\n");

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

RGBStream::RGBStream() {}
RGBStream::~RGBStream() {}
