#pragma once

#ifdef TXTSTRM_USE_FATFS
#include "ff.h"
#else
#include <sd.h>
#endif

// Includes the SD card routines
// #include <SD.h> // Don't, using a different module now
// Includes the LED display driver
#include "RGBStream.hpp"

/* Below follow the descriptions you may vary for fine tuning */
#define TXTSTRM_COLOR_CACHE_SIZE 1	// 20 seems aplenty

/* Maximum possible size of the display string */
#define TXTSTRM_MAX_DISPLAY_STR_LEN 64

/* How are two strings separated in file? Below way! */
#define TXTSTRM_DISPLAY_STR_DELIMITER '\n'

/* Modifyable portion of code ends. Tinker ahead at your own risk */

// #define TXTSTRM_USE_FATFS
	// We use the FATFS module by chaN (elm-chan.org)

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
		NO_MATCHING_ENTRY,
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

	/* @brief: Initialise the SD card
	 * @args:
	 *	cs_pin: The chip select pin for the SD Card
	 *		(An SPI connection is made to the card)
	 */
	bool init(unsigned char cs_pin);

	/* @brief: Collect a string from a SD card file and save it in buffer
	 * @args:
	 * 	#offset_in_file: Look for the string from this offset - Canned
	 *	#line_length: Length of string; 0 for delimiter detect - ^
	 *	file_name: File name with complete path; NULL for default
	 */
//	bool collectStr(	int offset_in_file, unsigned short line_length,
//			char *file_name);
	bool collectStr(char *file_name);

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

	/* @brief: Display the given string using the presently loaded
	 *	font in it's default color
	 * @args:
	 *	str: The string to display
	 */
	bool displayStr(char *str);

	struct ptr_ret {	// A way to return a ptr with a rv
		char *ptr;
		char rv;
	};

	struct color_save {
		char name[TXTSTRM_COLOR_NAME_SIZE];	// NULL for empty
		struct color_desc backg_col;
		struct color_desc foreg_col;
	};

private:
	struct ptr_ret process_path(	char *file_str,
					char *ext, char *def_path,
					char *str_buf, char buflen);

	char find_col_cache(char *name);	// Pass NULL to find empty

	/* Colors are allocated and stored here. Realloc is used upon
	 * additions or deletions
	 */
	struct color_store {
		char name[TXTSTRM_COLOR_NAME_SIZE];
		struct RGBStream::color_desc backg_col;
		struct RGBStream::color_desc foreg_col;
	} *col_st;

	/* Just one font'll be kept cache'd */
	struct font_desc *font_cache;

	/* Finds us the color in cache */
	char find_color_cache(char *col_name);

	/* We'll keep a static cache of color descriptions */
	struct color_save col_save[TXTSTRM_COLOR_CACHE_SIZE];

	// The display string is kept here after allocation
	char *disp_str;

	char *def_txt_file_path;	// Default text file path
	char *def_fnt_dir;	// Default dir to look for font files in
	char *def_col_dir;	// Default dir to look for color files in

#ifdef TXTSTRM_USE_FATFS
	FATFS fs;	// The mounted volume
#endif

#ifdef TXTSTRM_TEST
public:
	bool collect_and_print_str(char *file_name);
	/* Collect demofont and check it's integrity.
	 * Demofont is a font with name demofont and file name demofont.sft;
	 * it has the first two chars 'O' and 'K'.
	 */
	bool demofont_test();

	/* Collect demo-colors and check their integrity.
	 * Democolors are named 'blknwhte' and 'dutch';
	 * Both in the same file would be a better test.
	 */
	bool democolor_test();
#else
#warning "TextStream test functions disabled"
#endif
};
