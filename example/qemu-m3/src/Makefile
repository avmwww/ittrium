################################################################################
include ../Makefile.rules
#
vpath %.c src
vpath %.h src ../include
#
SRCS = \
	startup.c \
	uart.c \
	syscalls.c \
	test.c
#
sub: $(LISTDIR) $(OBJS)

include Makefile.dep
