################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = test55x.out
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) lnkx.cmd

$(SUBDIRS):
	$(MAKE) -C $@
# Naming our phony targets
.PHONY: $(SUBDIRS)

