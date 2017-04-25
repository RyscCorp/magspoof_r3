MCU=attiny85
AVRDUDEMCU=t85
CPU_SPEED=-DF_CPU=8000000UL
CC=avr-gcc
CFLAGS=-g -std=c99 -Os -Wall -w -mcall-prologues -mmcu=$(MCU) $(CPU_SPEED)
OBJ2HEX=avr-objcopy
AVRDUDE=avrdude
PROGRAMMER ?= usbtiny
TARGET=magspoof

$(TARGET).hex: $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex
	rm -f $(TARGET)

install: $(TARGET).hex
	$(AVRDUDE) -p $(AVRDUDEMCU) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex

noreset: all
	$(AVRDUDE) -p $(AVRDUDEMCU) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex

test:
	./test.sh

fuse:
	$(AVRDUDE) -p $(AVRDUDEMCU) -c $(PROGRAMMER) -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

firstprog: fuse install
	$(AVRDUDE) -p $(AVRDUDEMCU) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex

clean :
	rm -f *.hex *.obj *.o
