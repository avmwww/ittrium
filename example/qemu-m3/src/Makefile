################################################################################
include ../Makefile.rules
#
KERNEL_CONFIG_H := $(abspath ../include/kernel_config.h)

vpath %.c src
vpath %.h src ../include
#
SRCS = \
	startup.c \
	uart.c \
	syscalls.c \
	test.c

include $(KERNEL_DIR)/services/services.mk

sub: $(LISTDIR) $(OBJS)

include Makefile.dep
