RM = rm -rfv
MKDIR = mkdir -p 

INPUT_FILE_NAME = \
main

OUTPUT_FILE_NAME =\
I2CAddressScanner

SRC +=            \
$(INPUT_FILE_NAME).c

OBJ_DIR =         \
obj

HEX     =         \
$(OUTPUT_FILE_NAME).hex

ELF     =         \
$(OUTPUT_FILE_NAME).elf



$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

.PHONY: all dude clean

all : $(OBJ_DIR)
	avr-gcc -Os -g -mmcu=atmega328p -I/usr/lib/avr ./$(SRC) -o $(OBJ_DIR)/$(ELF)
	avr-objcopy -O ihex -R .eeprom $(OBJ_DIR)/$(ELF) $(OBJ_DIR)/$(HEX)

dude :
	avrdude -C /etc/avrdude.conf -c arduino -p atmega328p -D wiring -P /dev/ttyACM0 -b 115200 -U flash:w:$(OBJ_DIR)/$(HEX):a

clean :
	$(RM) $(OBJ_DIR)/*
