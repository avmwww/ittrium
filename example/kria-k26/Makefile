################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = ittrium-kria-k26.elf
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-Tlnk.ld
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: $(SUBDIRS)
