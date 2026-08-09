# Lists kernel objects for EWARM / custom IAR makefile
KERNEL_DIR ?= $(PWD)/../../..
KERNEL_DEV = m16c62
SRCS = $(KERNEL_DIR)/kernel/ittrium.c \
       $(KERNEL_DIR)/config/$(KERNEL_DEV)/cpu_support.c \
       $(KERNEL_DIR)/config/$(KERNEL_DEV)/cpu_support.asm
all:
	@echo "$(SRCS)"
