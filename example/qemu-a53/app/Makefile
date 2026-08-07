################################################################################
# Build a position-independent ELF for: run /hello.elf (romfs) or /data/...
#
#   make -C example/qemu-a53/app
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
BLOB_O = hello_elf_bin.o

all: $(TARGET) $(BLOB_O)

$(TARGET): main.c $(LDSCRIPT)
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o $@

# Linkable blob for romfs_add_builtin (symbols _binary_hello_elf_{start,end,size})
$(BLOB_O): $(TARGET)
	$(OBJCOPY) -I binary -O elf64-littleaarch64 -B aarch64 \
		--rename-section .data=.rodata,alloc,load,readonly,data,contents \
		$< $@

clean:
	rm -f $(TARGET) $(BLOB_O)

.PHONY: all clean
