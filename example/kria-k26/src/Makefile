################################################################################
include ../Makefile.rules

KERNEL_CONFIG_H := $(abspath ../include/kernel_config.h)

vpath %.c src

SRCS = \
	test.c \
	uart.c \
	gem_net.c \
	syscalls.c \
	string.c \

include $(KERNEL_DIR)/services/services.mk

sub: $(LISTDIR) $(OBJS)

include Makefile.dep
