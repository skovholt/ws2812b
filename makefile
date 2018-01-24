#	This is a makefile. It's invoked by invoking 'make' utility while in the same
#	directory as this file. (i.e. in bash, change directory to this file's and call 'make')

#	The following parameters should be edited to appropriate values

#	Set this to indicate the path where the compiler is located. If Arduino is installed on 
#	your Computer, you may edit this path to where the compiler may be found inside the Arduino 
#	application folder.
#
#	TIP: Incase you are trying to use the compiler inside Arduino, The path may be something 
#	like this: <Path To Arduino's Main Folder>/Java/hardware/tools/bin/

ARDUINO_ROOT_PATH = /Applications/Arduino.app/Contents/

BIN_DIR = $(addsuffix Java/hardware/tools/avr/bin/, $(ARDUINO_ROOT_PATH))
ARDUINO_INC_DIR = $(addsuffix Java/hardware/arduino/avr/cores/arduino, $(ARDUINO_ROOT_PATH))
ARDUINO_PINS_INC_DIR = $(addsuffix Java/hardware/arduino/avr/variants/mega, $(ARDUINO_ROOT_PATH))

COMPILER_NAME = avr-gcc

COMPILER_FLAGS = -mmcu=atmega328p

TEST_SRC_DIR = TextStream
SRC_DIR = src RGBStream TextStream SPI SD SD/utilities
INC_DIR = $(SRC_DIR) $(ARDUINO_INC_DIR) $(ARDUINO_PINS_INC_DIR)

BUILD_DIR = build

SRC = $(SRC_DIR)/*.cpp
TEST_SRC = $(TEST_SRC_DIR)/*.cpp

OBJ = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(SRC)))
TEST_OBJ = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(TEST_SRC)))

COMPILER = $(addprefix $(BIN_DIR), $(COMPILER_NAME))
INC = $(addprefix -I, $(INC_DIR))

$(BUILD_DIR)/%.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) $(INC) -c $^ -o $@

ctest: $(TEST_OBJ) build

build:
	mkdir build

.PHONY: clean

clean:
	rm build/*
