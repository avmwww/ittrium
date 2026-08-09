################################################################################
include ../Makefile.rules
#
KERNEL_CONFIG_H := $(abspath ../include/kernel_config.h)

vpath %.c src
vpath %.S src
vpath %.h src ../include
#
SRCS_HAL = \
		   stm32l1xx_hal.c \
		   stm32l1xx_hal_cortex.c \
		   stm32l1xx_hal_gpio.c \
		   stm32l1xx_hal_rcc.c \
		   system_stm32l1xx.c \
		   startup_stm32l100xba.s

SRCS = $(SRCS_HAL)

SRCS += \
	test.c \
	syscalls.c \
	uart.c

include $(KERNEL_DIR)/services/services.mk

sub: $(LISTDIR) $(OBJS)

include Makefile.dep
