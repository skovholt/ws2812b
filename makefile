#	This is a makefile. It's invoked by invoking 'make' utility while in the same
#	directory as this file. (i.e. in bash, change directory to this file's and call 'make')

#	The following parameters should be edited to appropriate values:

ARDUINO_ROOT_PATH = /Applications/Arduino.app/Contents/

#	Below parameters are probably OKAY but may need some adjustment in case of errors

#	Set this to indicate the path where the compiler is located. If Arduino is installed on 
#	your Computer, you may edit this path to where the compiler may be found inside the Arduino 
#	application folder.
#
#	TIP: Incase you are trying to use the compiler inside Arduino, The path may be something 
#	like this: <Path To Arduino's Main Folder>/Java/hardware/tools/bin/

BIN_DIR = $(addsuffix Java/hardware/tools/avr/bin/, $(ARDUINO_ROOT_PATH))
AVRDUDE_CONF_FILE = $(addsuffix Java/hardware/tools/avr/etc/avrdude.conf, $(ARDUINO_ROOT_PATH))

#	Set this to indicate where the .. - UPDATE - below defines not needed anymore
ARDUINO_SRC_DIR = $(addsuffix Java/hardware/arduino/avr/cores/arduino, $(ARDUINO_ROOT_PATH))
ARDUINO_INC_DIR = $(ARDUINO_SRC_DIR)
ARDUINO_PINS_INC_DIR = $(addsuffix Java/hardware/arduino/avr/variants/mega, $(ARDUINO_ROOT_PATH))

#	Don't edit anything below unless you intend to tinker.

COMPILER_NAME = avr-gcc

HOST_COMPILER = gcc

COMPILER_FLAGS = -mmcu=atmega328p -DF_CPU=8000000 -O -std=gnu99 -DTXTSTRM_TEST=1

# 	FontGen related below:
FONTGEN_DIR = FontGen
COLGEN_DIR = $(FONTGEN_DIR)
FONTGEN_FILE = font_out.c
COLGEN_FILE = col_out.c
FONTGEN_SRC = $(addprefix $(FONTGEN_DIR)/, $(FONTGEN_FILE))
COLGEN_SRC = $(addprefix $(COLGEN_DIR)/, $(COLGEN_FILE))
FONTGEN_TARGET = clicktogetfontfile
COLGEN_TARGET = clicktogetcolorfile
#	FontGen related end.

SRC_DIR = RGBStream TextStream
INC_DIR = $(SRC_DIR) uart
LIB_DIR = lib

SRC = $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))
ARDUINO_SRC = $(wildcard $(ARDUINO_SRC_DIR)/*.c) $(wildcard $(ARDUINO_SRC_DIR)/*.cpp)

TEST_SRC_DIR = TextStream
TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.cpp)

BUILD_DIR = build
BUILD_TREE = $(addprefix $(BUILD_DIR)/, $(SRC_DIR))

OBJ = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(SRC)))
TEST_OBJ = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(TEST_SRC)))

LIB = $(wildcard $(LIB_DIR)/*.a)

DEFINES = TXTSTRM_TEST

COMPILER = $(addprefix $(BIN_DIR), $(COMPILER_NAME))
AVRDUDE = $(addprefix $(BIN_DIR), avrdude)
OBJCOPY = $(addprefix $(BIN_DIR), avr-objcopy)

INC = $(addprefix -I, $(INC_DIR))
DEF = $(addprefix -D, $(DEFINES))

$(BUILD_DIR)/%.o: %.cpp build
	$(COMPILER) $(COMPILER_FLAGS) $(INC) $(DEF) -c $^ -o $@

all: $(OBJ) build

test: src/test.cpp all
	@echo "Builing.."
	$(COMPILER) $(COMPILER_FLAGS) $(INC) $(OBJ) $(LIB) uart/uart.c $< -o build/test_prog
	$(OBJCOPY) -j .text -j .data -O ihex build/test_prog test_prog.hex

ctest: $(TEST_OBJ) build

fontgen: $(FONTGEN_TARGET) $(COLGEN_TARGET)

.PHONY: clean $(FONTGEN_TARGET) $(COLGEN_TARGET) test_upload

$(FONTGEN_TARGET):
	@echo "Acting on:" $(FONTGEN_SRC)
	$(HOST_COMPILER) -o $(FONTGEN_TARGET) $(FONTGEN_SRC)
	./$(FONTGEN_TARGET)

$(COLGEN_TARGET):
	@echo "Acting on:" $(COLGEN_SRC)
	$(HOST_COMPILER) -o $@ $(COLGEN_SRC)
	./$(COLGEN_TARGET)

test_upload:
	@echo "Uploading.."
	$(AVRDUDE) -p m328p -b 74880 -C $(AVRDUDE_CONF_FILE) -c usbasp -Ulfuse:w:0xE2:m -Uflash:w:test_prog.hex

build: $(BUILD_TREE)

$(BUILD_TREE):
	mkdir -p $@

clean:
	rm -r build/*
