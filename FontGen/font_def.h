#ifndef __FONT_DEF_H__
#define __FONT_DEF_H__

#define MAX_WIDTH_HARD_LIMIT 16		// Width is flexible for each character but can't exceed this limit
#define LED_STRIP_NUM 10		// How many led strips are connected in parallel - equals height
					// A smaller font height may still be described but not larger
#define FONT_NAME_SIZE 8

#if MAX_WIDTH_HARD_LIMIT <= 8	// visual description is saved as a bitfield array with
typedef char vis_desc_row_t;	// rows of type vis_desc_row_t
#else
typedef short int vis_desc_row_t;
#endif

#if LED_STRIP_NUM <= 8		// byte representation is internal representation of the
typedef char byte_rep_row_t;	// visual description. It's row size should accomodate
#else				// one bit for each LED STRIP
typedef short int byte_rep_row_t;
#endif

#define NUM_FONTS 1

struct color_rgb {	// GRB is the data order for WS2812x
	char green;
	char red;
	char blue;
};

struct char_vis_desc {	// Visual definition of a character
	int utf8_val;	// The "num" of the char, being a short max 65535 values
	char width;	// Width can be anything greater than zero and less than MAX_WIDTH_HARD_LIMIT
	int vis_desc[LED_STRIP_NUM];	// visual description as bit field, int so max 32 width
};

struct char_desc {	// Formed after processing char_vis_desc
// This structure describes the exact bits that need to be uploaded (but
// after adding color information to the bit pattern)
	int utf8_val;
	char width;	// Width is feed array length
	short int feed[]; // Can be char too but def. short for alignment
//	union {	// Depends upon height if chars or shorts
//		char feed_chars[];
//		short int feed_shorts[];
//	} feed;
};

struct font_desc {	// A font_def(inition) consits mainly of char_defs and some general properties
	char name[FONT_NAME_SIZE];
	char height;	// Height is the same for all font, can't vary by characters
	struct color_rgb default_backg;
	struct color_rgb default_foreg;
	struct char_vis_desc *char_desc_array;
};

struct font_desc_arch {	// This is the storage format (in disc) of a font definition
			// This goes onto an SD card and the firmware picks it up
	char name[FONT_NAME_SIZE];
	char height;	// Can be redefined to lower (but not higher)
	struct color_rgb default_backg;
	struct color_rgb default_foreg;
	struct char_desc char_desc_array[];
};

#define END_CHAR_DESC (struct char_vis_desc) {\
			.width = 0,\
			.utf8_val = '\0',\
			.vis_desc = {0}\
			}

#define FONT_DEF (struct font_desc[]) {\
	(struct font_desc) {\
		.name = {'d', 'e', 'm', 'o', 'f', 'o', 'n', 't'},\
		.height = 8,\
		.default_backg = (struct color_rgb) {0, 0, 0},\
		.default_foreg = (struct color_rgb) {140, 140, 140},\
		.char_desc_array = (struct char_vis_desc[]) {\
			(struct char_vis_desc) {\
				.width = 8,\
				.utf8_val = 'A',\
				.vis_desc = {	0b00011000	,\
						0b00111100	,\
						0b01100110	,\
						0b01111110	,\
						0b11000011	,\
						0b10000001	,\
						0b10000001	,\
						0b10000001	,\
				}\
			},\
			(struct char_vis_desc) {\
				.width = 8,\
				.utf8_val = 'a',\
				.vis_desc = {	0b00000000	,\
						0b00000000	,\
						0b00111110	,\
						0b11000011	,\
						0b11000011	,\
						0b11000011	,\
						0b11000111	,\
						0b01111011	,\
				}\
			},\
			(struct char_vis_desc) {\
				.width = 3,\
				.utf8_val = '|',\
				.vis_desc = {	0b010	,\
						0b010	,\
						0b010	,\
						0b010	,\
						0b010	,\
						0b010	,\
						0b010	,\
						0b010	,\
				}\
			},\
			END_CHAR_DESC\
		}\
	}\
}

#endif
