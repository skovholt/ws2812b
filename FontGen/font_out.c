// This	program takes the definitions in font_def.h and outputs a
// file understandable by compatible firmware (by employing
// RGBStream and TextStream classes)

// file save format is .sft (contraction of SolomonFonT)

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "font_def.h"

#define FILE_FORMAT_STR ".sft"

// For the Arduino SD lib this is the limit
#define FILE_NAME_SIZE 13	

// Just a safe number so the stack in 
// make_char_feed_desc doesn't run-out
#define CHAR_DESC_MAX_SIZE 16 * 32 + 512

struct char_desc * make_char_feed_desc(	struct char_vis_desc *cvd,
					char font_height)
{
	// This holds the structure except the feed array at the end
	struct char_desc char_desc_head;
	// Here the struct above is copied and feed array appended
	static char empty_bytes[CHAR_DESC_MAX_SIZE];
	
	// Set the non array part of the struct
	char_desc_head = (struct char_desc) {
		.utf8_val = cvd->utf8_val,
		.width = cvd->width
	};

	// Copy the structure contents to empty_bytes
	// Now only appending the feed array remains
	memcpy(empty_bytes, &char_desc_head, sizeof(struct char_desc));

	for(char i = cvd->width - 1; i >= 0; --i) {

		short int feed_element;
		feed_element = 0;

		for(char j = 0; j < font_height; j++) {

			feed_element |= (cvd->vis_desc[font_height - 1\
					- j] & (1 << i)) ? (1 << j) : 0;
		}

		if(font_height > 8) {
			memcpy(empty_bytes + sizeof(struct char_desc)\
			+ i * sizeof(short int), &feed_element,
			sizeof(short int));
		} else {
			memcpy(empty_bytes + sizeof(struct char_desc)\
			+ i * sizeof(char), ((char *) &feed_element),
			sizeof(char));
		}
	}

	return (struct char_desc *) empty_bytes;
}

int main(int argc, char *argv[])
{
	for(char i = 0; i < NUM_FONTS; i++) {
		char file_name[FILE_NAME_SIZE];
		struct char_desc *pcd;
		struct font_desc_arch font_desc_save;
		int rv;
		FILE *target;

		// Making a file-name first
		strncpy(	file_name, FONT_DEF[i].name,
				FILE_NAME_SIZE\
				- sizeof(FILE_FORMAT_STR));
		file_name[FILE_NAME_SIZE - sizeof(FILE_FORMAT_STR)]\
				= 0;	// Safety NULL termination
		strcat(file_name, FILE_FORMAT_STR);

		// Creating a file with the above filename 
		// in the current directory
		target = fopen(file_name, "w");
		if(target == NULL) {
			printf(	"Failed to create \
					a new file\n");
			return -1;
		}

		font_desc_save = (struct font_desc_arch) {
			.height = FONT_DEF[i].height,
			.default_backg = FONT_DEF[i].default_backg,
			.default_foreg = FONT_DEF[i].default_foreg
		};

		memcpy(font_desc_save.name, FONT_DEF[i].name,
			FONT_NAME_SIZE);

		printf("Processing font %.8s\n", font_desc_save.name);

		fwrite(&font_desc_save, sizeof(struct font_desc_arch),
			1, target);

		for(	struct char_vis_desc * pcvd = \
			FONT_DEF[i].char_desc_array;
			pcvd->utf8_val != 0;
			pcvd++)
//			pcvd = ((char *) pcvd) + \
//				sizeof(struct char_vis_desc) + \
//				sizeof(int) * FONT_DEF[i].height)
		{
			printf("\tprocessing char %c.. ", (char) pcvd->utf8_val);
			pcd = make_char_feed_desc(	\
					pcvd,
					FONT_DEF[i].height);
			printf("%c done\n", (char) pcd->utf8_val);
			rv = fwrite(	pcd, sizeof(struct char_desc)\
				+ ((FONT_DEF[i].height > 8) ? \
				pcd->width * 2 : pcd->width),
				1, target);
			if(rv != 1) {
				printf("fwrite() fail!!!\n");
				printf("%s", strerror(errno));
				return -1;
			}
		}

		fclose(target);
	}
	
	return 0;
}
