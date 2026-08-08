include ../Makefile.rules

vpath %.c $(KERNEL_DIR)/kernel $(KERNEL_DIR)/config/$(KERNEL_DEV)
vpath %.h ../include $(KERNEL_DIR)/include $(KERNEL_DIR)/config/$(KERNEL_DEV)

SRCS = ittrium.c

include $(KERNEL_DIR)/config/Makefile.dep

sub: $(LISTDIR) $(OBJS)
