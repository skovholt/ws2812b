#include <stdlib.h>
#include <string.h>

#include "TextStream.hpp"

#ifdef TXTSTRM_USE_FATFS
#include "ff.h"
#else
#include <sd.h>
#endif

#ifdef TXTSTRM_TEST
#warning "TXTSTRM_TEST is defined"
#include "uart.h"
#define DEBUG_PRINT(M) uart0_puts(M)
#else
#define DEBUG_PRINT(M)
#endif

/* The file extension for file font devised by the illustrious
 * creator Solomon Candy.
 */
#define TXTSTRM_FNT_FILE_EXT ".sft"

/* The file extension for a color description file. Nothing
 * exemplary here, though still devised by the Solomon as above.
 */
#define TXTSTRM_COLOR_FILE_EXT ".scl"

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
static const int MAX_FILE_PATH_LEN = 128;

static const char FORW_SLASH = '/';

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
#ifdef TXTSTRM_USE_FATFS
	/* Initialise the ff module */
	FRESULT rs;

	DEBUG_PRINT("TextStream initialising..\n");

	rs = f_mount(&fs, "", 1);

	if(rs != FR_OK) return false;
#else
	return SD.begin(cs);
#endif
	
	return true;
}

/* A function that is supposed to fast
 * concatenate a series of strings.
 */
struct TextStream::ptr_ret TextStream::process_path(
				char *fl_str, char *ext,
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
			return (struct TextStream::ptr_ret) \
						{fl_str, 0};
		else		// Non-sense input
			return (struct TextStream::ptr_ret) \
				{NULL, NONSENSE_INPUT};
	}

	/* If here, file name provided possibly, with or
	 * without extension.
	 */

	len = strlen(fl_str) + strlen(def_path) + 1;
		// + 1 for NULL termination

	if(c_mark) {	// File extension provided
		// Just prefix with default path
		if(buflen < len) {
			return (struct TextStream::ptr_ret)\
			{NULL, BUFFER_INSUFF};//No space
		} else {
			strcpy(str_buf, def_path);
			strcat(str_buf, fl_str);
			return (struct TextStream::ptr_ret)\
						{str_buf, 0};
		}
	} else {
		// Prefix def path and suffix extension
		if(buflen < len + strlen(ext)) {
			return (struct TextStream::ptr_ret)\
			{NULL, BUFFER_INSUFF};//No space
		} else {
			strcpy(str_buf, def_path);
			strcat(str_buf, fl_str);
			return (struct TextStream::ptr_ret)\
						{str_buf, 0};
		}
	}

}

bool TextStream::loadFont(char *fnt_fl_str)
{
#ifdef	TXTSTRM_USE_FATFS
	FIL fnt_f;
	FRESULT rs;
	UINT bytes_read;
#else
	SDFile fnt_f;		// Font file object
#endif
	struct ptr_ret ret;	// Return value goes here
	struct font_desc *temp_font_cache;
	unsigned short int font_desc_size;
	char fl_name_buf[MAX_FILE_PATH_LEN];
			// Buffer on stack to keep the file name

	ret = process_path(	fnt_fl_str, TXTSTRM_FNT_FILE_EXT,
				def_fnt_dir,
				fl_name_buf, MAX_FILE_PATH_LEN);

	if(ret.rv != 0) return false;	// Path not returned; err

#ifdef TXTSTRM_USE_FATFS
	rs = f_open(&fnt_f, ret.ptr, FA_OPEN_EXISTING);
	if(rs != FR_OK) return false;
#else
	fnt_f = SD.open(ret.ptr, FILE_READ);
	if(!fnt_f) return false;	// File couldn't be opened
	font_desc_size = fnt_f.size();
#endif

	temp_font_cache = (struct font_desc *) malloc(font_desc_size);

	if(!temp_font_cache) {	// Couldn't allocate mem
		goto ERROR;
	}

#ifdef TXTSTRM_USE_FATFS
	font_desc_size = f_size(&fnt_f);

	rs = f_read(&fnt_f, temp_font_cache, font_desc_size, 
							&bytes_read);
	if(rs != FR_OK) {
#else
	if(fnt_f.read(temp_font_cache, font_desc_size) \
						!= font_desc_size) {
#endif
		free(temp_font_cache);
		goto ERROR;
	} else {
#ifdef	TXTSTRM_USE_FATFS
		f_close(&fnt_f);
#else
		fnt_f.close();
#endif
		free(font_cache);
		font_cache = temp_font_cache;
		return true;
	}

ERROR:
#ifdef	TXTSTRM_USE_FATFS
		f_close(&fnt_f);
#else
		fnt_f.close();
#endif
		return false;
}

char TextStream::find_color_cache(char *name)
{
	if(name == NULL) {
		name = "";
	}

	for(char i = 0; i < TXTSTRM_COLOR_CACHE_SIZE; i++) {
		if(strncmp(	name, col_save[i].name, 
				TXTSTRM_COLOR_NAME_SIZE) == 0) {

			return i;
		}
	}

	return NO_MATCHING_ENTRY;
}

bool TextStream::loadColor(char *clr_str)
{
	/* Not required, gleanColors() is sufficient */
	return false;	// Implement later
}

bool TextStream::gleanColors(char *dir_path)
{
#ifdef TXTSTRM_USE_FATFS
	DIR dr;
	FIL f_col;
	FILINFO f_info;
	FRESULT rs;
	char loc;	// saves location in a big string
	UINT saved_bytes;
#else
	SDFile c_dir;	// dir with color files
	SDFile fl;	// Temp Class to view an opened file
#endif
	char ext_fmt[sizeof(TXTSTRM_COLOR_FILE_EXT) + 1];
		// specifies a file ending in EXT
	char f_pth[MAX_FILE_PATH_LEN];

	char *f_name;	// to store file name ptr
	char *mark;	// useful below :-D
	unsigned short save_index;	// This too :-|
	char num_col;	// number of colors in the opened file

#ifdef TXTSTRM_USE_FATFS
	ext_fmt[0] = '*';	// Making a pattern string
	strcpy(ext_fmt + 1, TXTSTRM_COLOR_FILE_EXT);

	// Preparing a prefix string, rest in loop
	loc = strlcpy(f_pth, dir_path, MAX_FILE_PATH_LEN);
		// This is not num_col!! Just frugality.
	f_pth[loc] = FORW_SLASH;

	rs = f_findfirst(&dr, &f_info, dir_path, ext_fmt);

	while(rs == FR_OK && f_info.fname[0]) {
		mark = strcpy(f_pth + loc + 1,
				f_info.fname);
		rs = f_open(&f_col, f_pth, FA_OPEN_EXISTING);
		if(rs != FR_OK) goto ERROR;

		num_col = f_size(&f_col) \
				/ sizeof(struct color_save);

		for(num_col; num_col; num_col--) {

			save_index = find_color_cache(NULL);
			if(save_index == NO_MATCHING_ENTRY)
				goto ERROR;

			rs = f_read(	&f_col,
					col_save + save_index,
					sizeof(struct color_save),
					&saved_bytes);

			if(rs != FR_OK || (saved_bytes != \
				sizeof(struct color_save))) {
				col_save[save_index].name[0]\
					= '\0';
				goto ERROR;
			}
		}

		f_close(&f_col);
		rs = f_findnext(&dr, &f_info);		
		if(rs != FR_OK) goto ERROR;
	}

	return true;

ERROR:
	f_close(&f_col);
	f_closedir(&dr);
	return false;
}
#else
	c_dir = SD.open(dir_path);
	while((fl = c_dir.openNextFile())) {

		f_name = fl.name();

		mark = strstr(f_name, TXTSTRM_COLOR_FILE_EXT);

		if(mark) {	// We have a color def file

		num_col = fl.size() / sizeof(struct color_save);

		for(num_col; num_col; num_col--) {

			save_index = find_color_cache(NULL);

			if(save_index != NO_MATCHING_ENTRY) {

			if(	fl.read(col_save + save_index,
				sizeof(struct color_save)) != \
				sizeof(struct color_save)) {

				col_save[save_index].name[0] =\
					'\0';
				goto ERROR;
			}
					
			} else {
			// No more slots, return False
				goto ERROR;
			}
		}
		}

		fl.close();
	}

	c_dir.close();

	return true;
ERROR:
	fl.close();
	c_dir.close();

	return false;
}
#endif

bool TextStream::collectStr(char *file_str)
{
#ifdef TXTSTRM_USE_FATFS
	FIL fh;
	FRESULT rs;
	UINT i;
#else
	File fh;
	char i;
#endif
	struct ptr_ret fl_name_ret;
	char fl_name_buf[TXTSTRM_MAX_FILE_NAME_SIZE];
	char str_buf[TXTSTRM_MAX_DISPLAY_STR_LEN * 2];
		// We have enough memory on stack for two sentences;
		// necessary if we are to skip one through
	char j, k;

	fl_name_ret = process_path(	file_str, TXTSTRM_TXT_FILE_EXT,
			def_txt_file_path, fl_name_buf,
			sizeof(fl_name_buf));
	if(fl_name_ret.rv) return false;

#ifdef TXTSTRM_USE_FATFS
	rs = f_open(&fh, file_str, FA_OPEN_EXISTING);
	if(rs != FR_OK) return false;

	rs = f_lseek(&fh, (rand() * f_size(&fh) - \
			TXTSTRM_MAX_DISPLAY_STR_LEN) / RAND_MAX);
	if(rs != FR_OK) {
		f_close(&fh); return false;
	}

	rs = f_read(&fh, str_buf, sizeof(str_buf), &i);
	if(rs != FR_OK) {
		f_close(&fh); return false;
	}

	f_close(&fh);
#else
	fh = SD.open(fl_name_ret.ptr);
	if(!fh) return false;

	if(!fh.seek(((rand() * fh.size()) - TXTSTRM_MAX_DISPLAY_STR_LEN \
			 / RAND_MAX))) {
		// Review the coverage formula more throughly again !
		fh.close();
		return false;
	}

	i = fh.read(str_buf, sizeof(str_buf));
	
	fh.close();
#endif

	if(i < TXTSTRM_MAX_DISPLAY_STR_LEN) {
		return false;
	}

	for(j = 0; str_buf[j] != TXTSTRM_DISPLAY_STR_DELIMITER; j++) {
		if(j == i) 
			return false;
	}

	for(k = j; str_buf[k] != TXTSTRM_DISPLAY_STR_DELIMITER; k++) {
		if(k == i)
			return false;
	}

	if(disp_str) free(disp_str);
	disp_str = (char *) malloc(k - j + 1);	// +1 for null termination

	if(!disp_str) {
		return false;
	}

	memcpy(disp_str, str_buf + j + 1, k - j);
	disp_str[k] = '\0';

	return true;
}

bool TextStream::displayStr(char *str)
{
	char rv;

	rv = displayString(font_cache, str);
	if(rv) return false;

	return true;
}

#ifdef TXTSTRM_TEST
bool TextStream::collect_and_print_str(char *file_name)
{
	if(!collectStr(file_name)) {
		uart0_puts("Failed to collect string\n");
		return false;
	}

	// How to print without arduino !
	return true;
}

bool TextStream::demofont_test()
{
	bool b_rv;
	char rv;
	struct char_desc *c_d;

	b_rv = loadFont("demofont");
	if(!b_rv) {
		uart0_puts("loadFont failed!\n");
		return false;
	}

	rv = strncmp(font_cache->name, "demofont", 8);
	if(rv) {
		uart0_puts("Font name test failed!\n");
		return false;
	}

	c_d = (struct char_desc *) (((char *) font_cache) + sizeof(struct font_desc));
	if(c_d->utf8_val != 'K') {
		uart0_puts("Char check at index 0 failed!\n");
		return false;
	}

	c_d = (struct char_desc *) ((char *) c_d + sizeof(struct char_desc) + \
		c_d->width * ((c_d->width <= 8) ? sizeof(char) : sizeof(short)));
	if(c_d->utf8_val != 'O') {
		uart0_puts("Char check at index 1 failed!\n");
		return false;
	}

	return true;
}

bool TextStream::democolor_test()
{
	char rv;

	rv = gleanColors(def_col_dir);
	if(rv != 1) {
		uart0_puts("gleanColors() failed!\n");
	}

	rv = find_color_cache("blknwhte");
	if(rv == NO_MATCHING_ENTRY) {
		uart0_puts("blknwht font not found!\n");
		return false;
	}

	rv = find_color_cache("dutch");
	if(rv == NO_MATCHING_ENTRY) {
		uart0_puts("dutch font not found!\n");
		return false; 
	}

	return true;
}
#endif
