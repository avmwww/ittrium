#
# example gd32f350 project
#
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = gd32f350.elf
LDSCRIPT = gd32f3x0.ld
#
sub:  $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-T$(LDSCRIPT) -Wl,--defsym=STACK_SIZE=0x100
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@
# Naming our phony targets
.PHONY: $(SUBDIRS)

