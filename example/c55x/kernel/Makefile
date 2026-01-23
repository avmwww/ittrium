include ../Makefile.rules
#
vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.a55 $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h55 $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
#
SRCS = cpu_support.a55 ittrium.c
#
include Makefile.dep
#
sub: $(LISTDIR) $(OBJS)

