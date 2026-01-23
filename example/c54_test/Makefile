################################################################################
include Makefile.rules
#
SUBDIRS = Kernel src
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o  link.cmd

$(SUBDIRS):
	$(MAKE) -C $@
# Naming our phony targets
.PHONY: $(SUBDIRS)

