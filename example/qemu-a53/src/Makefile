################################################################################
include ../Makefile.rules

KERNEL_CONFIG_H := $(abspath ../include/kernel_config.h)

vpath %.c src

# --- application (always) ---
SRCS = \
	test.c \
	uart.c \
	virtio_net.c \
	syscalls.c \
	string.c \

# --- optional services (CFG_USE_* in kernel_config.h) ---
include $(KERNEL_DIR)/services/services.mk

sub: $(LISTDIR) $(OBJS)

include Makefile.dep
