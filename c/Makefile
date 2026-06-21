
CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
ICEPROG = iceprog
HEXDUMP = hexdump
HEXDUMP_ARGS = -v -e '1/4 "%08x" "\n"'

#CFLAGS=-Wall -Os -march=rv32i -mabi=ilp32 -ffreestanding -nostartfiles -flto
CFLAGS=-Wall -Os -march=rv32i -mabi=ilp32 -ffreestanding -flto -nostartfiles -nostdlib -fomit-frame-pointer -I/usr/lib/picolibc/riscv64-unknown-elf/include

HEADER = up5k_riscv.h acia.h spi.h flash.h clkcnt.h ili9341.h i2c.h printf.h oledrgb.h ina226.h

SOURCES = start.S main.c acia.c spi.c flash.c clkcnt.c ili9341.c i2c.c printf.c oledrgb.c ina226.c

main.elf: lnk-app.lds $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS)  -Wl,-Bstatic,-T,lnk-app.lds,--strip-debug -o $@ $(SOURCES) -lgcc

disassemble: main.elf
	$(OBJDUMP) -d main.elf > main.dis

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.hex: %.bin
	$(HEXDUMP) $(HEXDUMP_ARGS) $< >$@

clean:
	rm -f *.bin *.hex *.elf *.dis
