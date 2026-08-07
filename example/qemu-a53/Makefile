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
		-global virtio-mmio.force-legacy=false \
		-netdev user,id=net0,hostfwd=tcp::10007-:7 \
		-device virtio-net-device,netdev=net0,bus=virtio-mmio-bus.0 \
		-kernel $(TARGET)

.PHONY: $(SUBDIRS) run
