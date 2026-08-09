################################################################################
include Makefile.rules
TARGET = arm7tdmi.out
SUBDIRS = kernel src
#
# Placeholder tree so `make -C example/arm7tdmi` documents the layout.
# Full link needs IAR (see README).
sub: $(OBJDIR)
	@echo "Ready sources under example/arm7tdmi/ (IAR build — see README)"
$(OBJDIR):
	$(MKDIR) $(OBJDIR)
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS) sub
