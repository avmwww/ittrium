include ../Makefile.rules
#
vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.S $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h54 $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
#
SRCS =  cpu_support.S ittrium.c
#
include Makefile.dep
#
sub: $(LISTDIR) $(OBJS)

