#
vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV) ../src/src
vpath %.s $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
#
SRCS = ittrium.c cpu_support.s
#
include $(KERNEL_DIR)/config/Makefile.dep
#
sub: $(LISTDIR) $(OBJS)

