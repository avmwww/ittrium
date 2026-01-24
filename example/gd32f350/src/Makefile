################################################################################
include ../Makefile.rules
#
vpath %.c src
vpath %.c $(GD32_STD_PERIPH)/Source
vpath %.c $(GD32_CMSIS)/GD/GD32F3x0/Source/
vpath %.c $(GD32_USB_LIB)/driver/Source $(GD32_USB_LIB)/device/core/Source
vpath %.c $(GD32_USB_LIB)/device/class/cdc/Source
vpath %.h src ../include
#
CFLAGS += -I../drivers/include \

SRCS += \
	main.c \
	target.c \
	system_gd32f3x0.c \
	start.c

SRCS += \
       gd32f3x0_rcu.c \
       gd32f3x0_pmu.c \
       gd32f3x0_misc.c \
       gd32f3x0_exti.c \

SRCS += \
       usbd_core.c \
       usbd_enum.c \
       usbd_transc.c \
       drv_usb_core.c \
       drv_usb_dev.c \
       drv_usbd_int.c \
       cdc_acm_core.c \

#
sub:  $(LISTDIR)  $(OBJS)

include Makefile.dep

