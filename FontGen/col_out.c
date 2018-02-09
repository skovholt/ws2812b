/* Description in col_def.h */

#include <stdio.h>
#include <string.h>

#include "col_def.h"

#define COL_FILE_EXT_STR ".scl"

int main(int argc, char **argv)
{
	char len;
	char file_name[COLOR_NAME_SIZE + sizeof(COL_FILE_EXT_STR)];
	FILE *f_hdl;

	if(*(char *) (COLORS + sizeof(struct color_desc_arch)) == '\0') {
		// Just one color
		len = strnlen(COLORS[0].name, COLOR_NAME_SIZE);
		memcpy(file_name, COLORS[0].name, len);
		memcpy(file_name + len, COL_FILE_EXT_STR, 
						strlen(COL_FILE_EXT_STR));
	} else {
		printf("Please enter the name of the file, excluding "\
			"extension, that you would like for the color "\
			"file. \nThe name may be anything and isn't "\
			"crucial to the functionality but only for your"\
			"own convenience to distinguish between files.\n");

		printf("(Max eight chars allowed)\n");

		scanf("%8s", file_name);
		strcat(file_name, COL_FILE_EXT_STR);
	}
	
	f_hdl = fopen(file_name, "w");
	if(f_hdl == NULL) {
		printf("There is a problem creating the file.. Aborting.\n");
		return -1;
	}
	
	for(struct color_desc_arch *cd = COLORS; cd->name[0] != '\0'; cd++) {

		char rv;

		printf("Adding color %.8s\n", cd->name);

		rv = fwrite(cd, sizeof(struct color_desc_arch), 1, f_hdl);
		if(rv != 1) {
			printf("Error while writing.. Aborting.\n");
			fclose(f_hdl);
			return -1;
		}
	}

	puts("Done!\n");

	return 0;
}
