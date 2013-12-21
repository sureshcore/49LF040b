CC = msp430-gcc
MMCU = msp430g2553
FLAGS = -Os -Wall -g -mmcu=$(MMCU)
SOURCES = main.c uart.c 49LF040B.c

all:
	@echo GCC:
	$(CC) $(FLAGS) -o out.elf $(SOURCES)
	@echo -------------------------------------------------------------------------------
	@echo -------------------------------------------------------------------------------
	@echo OBJCOPY:
	msp430-objcopy -O ihex out.elf out.hex

install: all
	@echo FLASHING
	MSP430Flasher -n $(MMCU) -v -w out.hex -i USB -z [VCC]
	del out.elf