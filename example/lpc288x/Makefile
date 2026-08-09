################################################################################
include Makefile.rules
TARGET = lpc288x.out
SUBDIRS = kernel src
#
# Placeholder tree so `make -C example/lpc288x` documents the layout.
# Full link needs IAR (see README).
sub: $(OBJDIR)
	@echo "Ready sources under example/lpc288x/ (IAR build — see README)"
$(OBJDIR):
	$(MKDIR) $(OBJDIR)
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS) sub
