// RGBStream class needs to be informed of a font and then given a line to
// set the LED array ablaze!

#pragma once

#define RGBSTREAM_FONT_CACHE_SIZE 5	// How many fonts to keep in store. Excess freed.

enum return_values {
	RGBSTREAM_FAIL = -1,
	RGBSTREAM_SUCCESS = 0,
};

/* A note on naming convention:
	This lib uses CamelCase for all the exposed functionalities but it may use
	underscore_seperated naming convention for internal use
 */
/* Class: RGBStream
 *	This class purports to provide an interface that absorbs font descriptions
 *	and then can stream any line (as char string) provided to a ??? LED
 *	string array. Options regarding foreground and background colour are also 
 *	provided. PORTC and PORTD on an atmega-48/88/168/328 (A/P) are required
 *	by the Class.
 *
 * How to use:
 *	The class methods are self explanatory. No initialisation is required.
 *
 * LED array configuration:
 *	The LED strips should be connected in parallel, their number indicated in 
 *	the appropriate '#define' and be of the same usable length each. Their 
 *	length should be able to accomodate the provided strings or the text would 
 *	be cut off. The bottom-most LED strip should be at PORTD pin0 with the 
 *	the higher strips taking consecutive higher pins, after PORTD pin7 the
 *	next higher strip should start at PORTC pin0, and then use the next higher 
 *	pin number for the next higher strip.
 */

class RGBStream {
public:
	struct color_desc {	// Colour description
		unsigned char green;
		unsigned char red;
		unsigned char blue;
	};

/*	Definition rejected because invalid
	struct char_desc {	// This defines a single char in font
		int utf8_val;	// int to accomodate multi byte UTF-8
		char width;	// In bytes or short, depends upon feed
		union {		// Depends upon font height
			char feed_bytes[];
			short int feed_shorts[];
		} feed;		// feed means the final byte array to 
				// be fed to the PORT.
	};
 */
	
	struct char_desc {	// This defines a single char in font
		int utf8_val;	// int to accomodate multi byte UTF-8
		char width;	// In bytes or short, depends upon feed
		short int feed[];	// Depends upon font height
	};

	struct font_desc {	// This contains font description and stores all the chars
		char height;
		struct color_desc def_foreg_color;
		struct color_desc def_backg_color;
	
		struct char_desc char_desc_ar[];
	};

	struct font_desc *current_font;
	static struct color_desc backg_color, foreg_color; 

	char displayString(struct font_desc *font, char *string);

	char displayStringWithColor(	struct font_desc *font,
					char *string, 	
					struct color_desc foreground_color,
					struct color_desc background_color);

	RGBStream();
	~RGBStream();
private:
	struct char_desc *find_char_desc_in_font(	int utf_char, 
							char feed_is_short,
							struct font_desc *font);

	char stream_char_feed(	struct char_desc *feed_char,
				char feed_is_short,
				struct color_desc foreg_color,
				struct color_desc backg_color);
};
