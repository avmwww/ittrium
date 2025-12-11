################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = test_stm32l1x.out
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-Tlnk.ld

$(SUBDIRS):
	$(MAKE) -C $@
# Naming our phony targets
.PHONY: $(SUBDIRS)

