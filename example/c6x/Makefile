################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = testc6x.out
#
sub: $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) lnk.cmd
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS)
