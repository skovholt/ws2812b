/* This generates a color description file to be used
 * with the TextStream class.
 */
#ifndef __COL_DEF_H__
#define __COL_DEF_H__

#include "font_def.h"	// Borrow the color_desc struct defined here

#define COLOR_NAME_SIZE 8
	// Important!!: Should correspond to the color name size
	// defined in TextStream class

struct color_desc_arch {	// Archival of color in (SD) memory format
	char name[COLOR_NAME_SIZE]; 
	struct color_rgb backg_col;
	struct color_rgb foreg_col;
};

#define COLORS_END (struct color_desc_arch) {\
	.name = {'\0'},\
	.backg_col = {0, 0, 0},\
	.foreg_col = {0, 0, 0},\
}

/* Define'd three demo colors below
 * Named black and white, dutch and dane.
 */

#define COLORS (struct color_desc_arch[]) {\
	(struct color_desc_arch) {\
		.name = {'b', 'l', 'k', 'n', 'w', 'h', 't', 'e'},\
		.backg_col = {0, 0, 0},\
		.foreg_col = {200, 200, 200}\
	},\
	(struct color_desc_arch) {\
		.name = {'d', 'u', 't', 'c', 'h', '\0'},\
		.backg_col = {200, 50, 50},\
		.foreg_col = {100, 200, 25}\
	},\
	(struct color_desc_arch) {\
		.name = {'d', 'a', 'n', 'e', '\0'},\
		.backg_col = {75, 75, 75},\
		.foreg_col = {25, 200, 25}\
	},\
	COLORS_END\
}

#endif
