################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = ittrium-qemu-m3.elf
#
sub: $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-Tlnk.ld
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

run: sub
	qemu-system-arm \
		-machine lm3s6965evb \
		-nographic \
		-kernel $(TARGET)

clean:
	$(RM) $(OBJDIR) $(LISTDIR) $(TARGET) $(TARGET).map

.PHONY: sub $(SUBDIRS) run clean
