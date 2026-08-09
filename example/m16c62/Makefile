################################################################################
include Makefile.rules
TARGET = m16c62.out
SUBDIRS = kernel src
#
# Placeholder tree so `make -C example/m16c62` documents the layout.
# Full link needs IAR (see README).
sub: $(OBJDIR)
	@echo "Ready sources under example/m16c62/ (IAR build — see README)"
$(OBJDIR):
	$(MKDIR) $(OBJDIR)
$(SUBDIRS):
	$(MAKE) -C $@
.PHONY: $(SUBDIRS) sub
