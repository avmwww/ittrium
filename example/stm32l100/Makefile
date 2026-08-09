################################################################################
include Makefile.rules
#
SUBDIRS = kernel src
TARGET = test_stm32l1x.out
RENODE ?= $(KERNEL_DIR)/third_party/renode_1.15.3_portable/renode
UART_LOG := $(abspath uart.log)
REPL := $(abspath renode/stm32l100.repl)
ELF := $(abspath $(TARGET))
RESC_TEST := renode/shell_test.resc
RESC_SHELL := renode/shell.resc
#
sub: $(OBJDIR) $(SUBDIRS)
	$(LNK) $(OBJDIR)/*.o -o $(TARGET) $(LDFLAGS) -Wl,-Tlnk.ld
	$(SIZE) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

$(RESC_TEST): renode/shell_test.resc.in
	sed -e 's|@PLATFORM@|@$(REPL)|g' \
	    -e 's|@ELF@|@$(ELF)|g' \
	    -e 's|@UARTLOG@|@$(UART_LOG)|g' \
	    $< > $@

$(RESC_SHELL): renode/shell.resc.in
	sed -e 's|@PLATFORM@|@$(REPL)|g' \
	    -e 's|@ELF@|@$(ELF)|g' \
	    $< > $@

run: sub $(RESC_SHELL)
	$(RENODE) --disable-xwt --console $(abspath $(RESC_SHELL))

stop:
	@-pkill -9 -f -- '--disable-xwt --console .*/stm32l100/renode/shell\.resc' 2>/dev/null; true
	@-pkill -9 -f -- '--disable-xwt .*/stm32l100/renode/shell\.resc' 2>/dev/null; true
	@rm -f .renode.pid
	@echo "renode stopped"

run-renode: sub $(RESC_TEST)
	rm -f $(UART_LOG)
	$(RENODE) --disable-xwt $(abspath $(RESC_TEST))
	@echo "---- uart.log ----"
	@cat $(UART_LOG)
	@rg -q "ittrium>" $(UART_LOG)
	@rg -q "commands:" $(UART_LOG)
	@rg -q "readme.txt" $(UART_LOG)
	@echo "shell smoke ok"

clean:
	$(RM) $(OBJDIR) $(LISTDIR) $(TARGET) $(TARGET).map uart.log $(RESC_TEST) $(RESC_SHELL) .renode.pid

.PHONY: sub $(SUBDIRS) run stop run-renode clean
