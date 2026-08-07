################################################################################
# Build a position-independent ELF for: run /data/hello.elf
#
#   make -C example/qemu-a53/app
#   # then copy hello.elf onto the guest FS (littlefs /data)
################################################################################
CROSS_COMPILE ?= aarch64-none-elf-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
READELF = $(CROSS_COMPILE)readelf

ITTRIUM ?= $(abspath ../../..)
LDSCRIPT = $(ITTRIUM)/services/elf/app.ld

CFLAGS = -mcpu=cortex-a53 -fPIE -ffreestanding -fno-builtin -Os -Wall
LDFLAGS = -nostdlib -Wl,-T$(LDSCRIPT) -Wl,-pie -Wl,-Bsymbolic \
	-Wl,--no-dynamic-linker -Wl,-z,max-page-size=16

TARGET = hello.elf

all: $(TARGET)
	$(READELF) -h -l -r $(TARGET) | head -60

$(TARGET): main.c $(LDSCRIPT)
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean
