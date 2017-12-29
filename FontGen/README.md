This hack uses C compiler's parsing to arrange the visual description of a font in
file font_def.h into a .sft file that can be understood by accompanying atmel firm
-ware.

Usage instructions:

• First add your font definitions to font_def.h

• Then compile the font_out.c file. Don't forget to include font_def.h

• Finally run the output of the compilation and you'll notice a .sft file created
  for every font defined in font_def.h

Stay tuned for more involved instructions on the use of this utility.
