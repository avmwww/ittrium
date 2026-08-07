################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = ittrium-qemu-a53.elf
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-Tlnk.ld
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

run: sub
	qemu-system-aarch64 \
		-machine virt,gic-version=2 \
		-cpu cortex-a53 \
		-nographic \
		-smp 1 \
		-m 128M \
		-kernel $(TARGET)

.PHONY: $(SUBDIRS) run
