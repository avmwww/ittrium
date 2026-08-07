#
vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV) ../src/src
vpath %.s $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.S $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
#
SRCS = ittrium.c
ifneq ($(wildcard $(KERNEL_DIR)/config/$(KERNEL_DEV)/cpu_support.S),)
SRCS += cpu_support.S
else
SRCS += cpu_support.s
endif
#
include $(KERNEL_DIR)/config/Makefile.dep
#
sub: $(LISTDIR) $(OBJS)

