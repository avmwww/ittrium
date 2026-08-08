################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = ittrium-linux-user
#
sub: $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS)
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

run: sub
	./$(TARGET)

clean:
	$(RM) $(OBJDIR) $(LISTDIR) $(TARGET) $(TARGET).map

.PHONY: $(SUBDIRS) run clean
