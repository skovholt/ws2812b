#pragma ONCE

#include <stdlib.h>
#include <string.h>

#include "TextStream.h"

/* The file extension for file font devised by the illustrious
 * creator Solomon Candy.
 */
#define TXTSTRM_FNT_FILE_EXT ".sft"

/* The file extension for a color description file. Nothing
 * exemplary here, though still devised by the Solomon as above.
 */
#define TXTSTRM_COL_FILE_EXT ".scl"

/* The .txt file extension
 */
#define TXTSTRM_TXT_FILE_EXT ".txt"

/* Max file name size is actually constrained by the SD lib
 */
#define TXTSTRM_MAX_FILE_NAME_SIZE 13

/* Define the max buffer size for file name and path purposes
 * to be kept on stack
 * It's defined as a file scope var (better than `#define`)
 */
static const short int MAX_FILE_PATH_LEN 128

static const short char FORW_SLASH '/'

TextStream::TextStream(	char *file_path,
			char *fonts_dir,
			char *colors_dir)
{
	/* Fill the default (fallback) values
	 * All values are optional. See .hpp
	 */
	def_txt_file_path = file_path;
	def_fnt_dir = fonts_dir;
	def_col_dir = colors_dir;

	return;
}

bool TextStream::init(unsigned char cs)
{
	/* Got nothing to do in init except start the
	 * SD Card utility
	 */
	return SD.begin(cs);
}

/* A prototype functions that is supposed to fast
 * concatenate a series of strings.
 */
struct ptr_ret process_path(	char *fl_str, char *ext,
				char *def_path,
				char *str_buf,
				char buflen)
{
	/* This function processes the arguments to give the
	 * desired path. The args must be in the following 
	 * order:
	 *	• The name or path of the file
	 *	• The file extension string 
	 *		(including the '.')
	 *	• The default file path for this type
	 */

	char fl_name_buf[MAX_FILE_PATH_LEN];	// on stack
	char len;
	char *c_mark;	// char ptr mark and
				// next mark

	c_mark = strstr(fl_str, ext);	// Check if extension
					// is input

	if(fl_str[0] == FORW_SLASH) {	// Full path provided
		if(c_mark) 	// Seems normal
			return (struct ptr_ret) {fl_str, 0};
		else		// Non-sense input
			return (struct ptr_ret) {NULL, NONSENSE_INPUT};
	}

	/* If here, file name provided possibly, with or
	 * without extension.
	 */

	len = strlen(fl_str) + strlen(def_path) + 1;
		// + 1 for NULL termination

	if(c_mark) {	// File extension provided
		// Just prefix with default path
		if(buflen < len) {
			return {NULL, BUFFER_INSUFF};	// No space
		} else {
			strcpy(str_buf, def_path);
			strcat(str_buf, fl_str);
			return {buf_len, 0};
		}
	} else {
		// Prefix def path and suffix extension
		if(buflen < len + strlen(ext)) {
			return {NULL, BUFFER_INSUFF};	//No space
		} else {
			strcpy(str_buf, def_path);
			strcat(str_buf, fl_str);
			return {buf_len, 0};
		}
	}

}

bool TextStream::loadFont(char *fnt_fl_str)
{
	SDFile fnt_f;		// Font file object
	struct ptr_ret ret;	// Return value goes here
	unsigned short int font_desc_size;
	char fl_name_buf[MAX_FILE_PATH_LEN];
			// Buffer on stack to keep the file name

	ret = process_path(	fnt_fl_str, TXTSTRM_FNT_FILE_EXT,
				fl_name_buf, MAX_FILE_PATH_LEN);

	if(ret.rv != 0) return false;	// Path not returned; err

	fnt_f = SDFile(ret.ptr, FILE_READ);

	if(!fnt_f) return false;	// File couldn't be opened

	font_desc_size = fnt_f.size();
	font_cache = (struct font_desc *) malloc(font_desc_size);

	if(!font_cache) {	// Couldn't allocate mem
		goto ERROR;
	}

	if(fnt_f.read(font_cache, font_desc_size) \
						!= font_desc_size) {
		goto ERROR;
	} else {
		fnt_f.close();
		return true;
	}

ERROR:
		fnt_f.close();
		return false;
}

bool TextStream::loadColor(char *clr_str)
{
	return false;	// Implement later
}

bool TextStream::gleanColors(char *dir_path)
{
	return false;	// Implement later
}

bool TextStream::collectStr(	char *)
{
	return false;	// Implement later
}

#ifdef TXTSTRM_TEST
bool TextStream::collect_and_print_str(	int offset, unsigned short file_len,
				char *file_name)
{
	if(!collect_str(offset, file_len, file_name)) {
		return false;
	}

	// How to print without arduino !
}
#endif
