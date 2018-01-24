#pragma once

// Includes the SD card routines
#include <SD.h>
// Includes the LED display driver
#include "RGBStream.hpp"

#define TXTSTRM_RND_CLR 1
#define TXTSTRM_RND_FNT 1 << 1
#define TXTSTRM_RND_LINE 1 << 2

#define TXTSTRM_COLOR_NAME_SIZE 8

#define TXTSTRM_DEF_FILE_PATH "/words.txt"
#define TXTSTRM_DEF_FONTS_DIR "/"
#define TXTSTRM_DEF_COLORS_DIR "/"

	/* Refer here for error codes
	 */
	enum ret_val {
		OK = 0,
		NONSENSE_INPUT = -128,
		BUFFER_INSUFF,
	};

class TextStream: public RGBStream 
{
public:
	/* @brief: Create object with given defaults */
	TextStream(	char *file_path = TXTSTRM_DEF_FILE_PATH,
			char *fonts_dir = TXTSTRM_DEF_FONTS_DIR,
			char *colors_dir = TXTSTRM_DEF_COLORS_DIR);
//			char flags = 	TXTSTRM_RND_CLR | TXTSTRM_RND_FNT | \
// Not text stream's job			TXTSTRM_RND_LINE);

	struct ptr_ret {	// A way to return a ptr with a rv
		char *ptr;
		char rv;
	};

	/* @brief: Initialise the SD card
	 * @args:
	 *	cs_pin: The chip select pin for the SD Card
	 *		(An SPI connection is made to the card)
	 */
	bool init(unsigned char cs_pin);

	/* @brief: Collect a string from a SD card file and save it in buffer
	 * @args:
	 * 	offset_in_file: Look for the string from this offset
	 *	line_length: Length of string; 0 for delimiter detect
	 *	file_name: File name with complete path; NULL for default
	 */
	bool collectStr(	int offset_in_file, unsigned short line_length,
			char *file_name);

	/* @brief: Load color from SD Card
	 * @args:
	 *	color_name_or_path: Either full path (beginning with '/') or
	 *		just name (searched in default path) of the color to 
	 *		load
	 */
	bool loadColor(char *color_name_or_path);

	/* @brief: Automatically load all the colors in a directory
	 * @args:
	 *	dir_path: Path of the dir containing colors; 
	 *		NULL for default path
	 */
	bool gleanColors(char *dir_path);

	/* @brief: Load the given font from an SD Card
	 * @args:
	 *	font_name_or_path: If just name, it's searched in def dir
	 */
	bool loadFont(char *font_name_or_path);

private:
	struct ptr_ret process_path(	char *file_str,
					char *ext, char *def_path,
					char *str_buf, char buflen);

	/* Colors are allocated and stored here. Realloc is used upon
	 * additions or deletions
	 */
	struct color_store {
		char name[TXTSTRM_COLOR_NAME_SIZE];
		struct RGBStream::color_desc backg_col;
		struct RGBStream::color_desc foreg_col;
	} *col_st = NULL;

	/* Just one font'll be kept cache'd */
	struct font_desc *font_cache = NULL;

	// The display string is kept here after allocation
	char *disp_str = NULL;

	char *def_txt_file_path;	// Default text file path
	char *def_fnt_dir;	// Default dir to look for font files in
	char *def_col_dir;	// Default dir to look for color files in

#ifdef TXTSTRM_TEST
public:
	bool collect_and_print_str(	int offset_in_file, unsigned short file_len,
				char *file_name);
#endif
};
