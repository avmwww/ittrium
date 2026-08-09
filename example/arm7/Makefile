################################################################################
include Makefile.rules
TARGET = arm7.out
SUBDIRS = kernel src
#
# Placeholder tree so `make -C example/arm7` documents the layout.
# Full link needs IAR (see README).
sub: $(OBJDIR)
	@echo "Ready sources under example/arm7/ (IAR build — see README)"
$(OBJDIR):
	$(MKDIR) $(OBJDIR)
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS) sub
