################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = testc67px.out
#
sub: $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) lnk.cmd
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS)
