# Lists kernel objects for EWARM / custom IAR makefile
KERNEL_DIR ?= $(PWD)/../../..
KERNEL_DEV = lpc288x
SRCS = $(KERNEL_DIR)/kernel/ittrium.c \
       $(KERNEL_DIR)/config/$(KERNEL_DEV)/cpu_support.c \
       $(KERNEL_DIR)/config/$(KERNEL_DEV)/cpu_support.s
all:
	@echo "$(SRCS)"
