include ../Makefile.rules
#
vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.s $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)
#
SRCS = ittrium.c cpu_support.s system_stm32l1xx.c startup_stm32l100xb.s
#
include Makefile.dep
#

sub: $(LISTDIR) $(OBJS)

