DEVICE = atmega328p
MCU = atmega328p
AVRDUDE_DEVICE = m328p
DEVICE ?= atmega168
MCU ?= atmega168
AVRDUDE_DEVICE ?= m168

LIB_DIR?=-L../..
CFLAGS=-g -Wall -mcall-prologues -mmcu=$(MCU) $(DEVICE_SPECIFIC_CFLAGS) -Os -I../.. $(LIB_DIR)
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

PORT ?= USB
AVRDUDE=avrdude
TARGET=main
OBJECT_FILES=main.o bargraph.o maze_solver.o follow_segment.o turn.o

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@ 

program: $(TARGET).hex
	$(AVRDUDE) -c usbtiny -B 1 -p $(AVRDUDE_DEVICE) -U flash:w:$(TARGET).hex
