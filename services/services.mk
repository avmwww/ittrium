################################################################################
# CFG_USE_* from kernel_config.h → optional SRCS (missing/0 = skip)
#
# Set before include: KERNEL_DIR, KERNEL_CONFIG_H
################################################################################

SERVICES  ?= $(KERNEL_DIR)/services
LITTLEFS  ?= $(KERNEL_DIR)/third_party/littlefs
LWIPDIR   ?= $(KERNEL_DIR)/third_party/lwip/src

# Read CFG_USE_xxx from kernel_config.h (missing → 0)
cfg_val = $(shell sed -n 's/^[[:space:]]*#define[[:space:]]\+$(1)[[:space:]]\+\([01]\).*/\1/p' $(KERNEL_CONFIG_H) 2>/dev/null | head -1)
CFG_USE_VFS    := $(if $(call cfg_val,CFG_USE_VFS),$(call cfg_val,CFG_USE_VFS),0)
CFG_USE_ROMFS  := $(if $(call cfg_val,CFG_USE_ROMFS),$(call cfg_val,CFG_USE_ROMFS),0)
CFG_USE_RAMFS  := $(if $(call cfg_val,CFG_USE_RAMFS),$(call cfg_val,CFG_USE_RAMFS),0)
CFG_USE_LFS    := $(if $(call cfg_val,CFG_USE_LFS),$(call cfg_val,CFG_USE_LFS),0)
CFG_USE_PROCFS := $(if $(call cfg_val,CFG_USE_PROCFS),$(call cfg_val,CFG_USE_PROCFS),0)
CFG_USE_SYSFS  := $(if $(call cfg_val,CFG_USE_SYSFS),$(call cfg_val,CFG_USE_SYSFS),0)
CFG_USE_SHELL  := $(if $(call cfg_val,CFG_USE_SHELL),$(call cfg_val,CFG_USE_SHELL),0)
CFG_USE_ELF    := $(if $(call cfg_val,CFG_USE_ELF),$(call cfg_val,CFG_USE_ELF),0)
CFG_USE_LWIP   := $(if $(call cfg_val,CFG_USE_LWIP),$(call cfg_val,CFG_USE_LWIP),0)

CFLAGS += -I$(SERVICES)/include
CFLAGS += -I$(KERNEL_DIR)/kernel

vpath %.c $(SERVICES)/drv
vpath %.c $(SERVICES)/vfs
vpath %.c $(SERVICES)/shell
vpath %.c $(SERVICES)/proc
vpath %.c $(SERVICES)/elf
vpath %.c $(SERVICES)/net
vpath %.c $(SERVICES)/net/arch

SRCS += device.c console.c netdev.c

ifeq ($(CFG_USE_VFS),1)
SRCS += vfs.c
endif

ifeq ($(CFG_USE_ROMFS),1)
SRCS += romfs.c
endif

ifeq ($(CFG_USE_RAMFS),1)
SRCS += ramfs.c
endif

ifeq ($(CFG_USE_LFS),1)
CFLAGS += -I$(LITTLEFS)
CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR -DLFS_NO_ASSERT
vpath %.c $(LITTLEFS)
SRCS += lfs_port.c lfs.c lfs_util.c
endif

ifeq ($(CFG_USE_PROCFS),1)
SRCS += procfs.c
endif

ifeq ($(CFG_USE_SYSFS),1)
SRCS += sysfs.c
endif

ifneq ($(filter 1,$(CFG_USE_PROCFS) $(CFG_USE_SYSFS)),)
SRCS += telemetry.c
endif

ifeq ($(CFG_USE_SHELL),1)
SRCS += shell.c lineedit.c shell_var.c shell_parse.c
ifeq ($(CFG_USE_LWIP),1)
SRCS += shell_net.c
endif
endif

ifeq ($(CFG_USE_ELF),1)
SRCS += elf_load.c
endif

ifeq ($(CFG_USE_LWIP),1)
CFLAGS += -I$(SERVICES)/net
CFLAGS += -I$(SERVICES)/net/arch
CFLAGS += -I$(LWIPDIR)/include
CFLAGS += -DLWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
vpath %.c $(LWIPDIR)/core
vpath %.c $(LWIPDIR)/core/ipv4
vpath %.c $(LWIPDIR)/api
vpath %.c $(LWIPDIR)/netif
SRCS += \
	sys_arch.c \
	net_init.c \
	netif_netdev.c \
	init.c \
	def.c \
	dns.c \
	inet_chksum.c \
	ip.c \
	mem.c \
	memp.c \
	netif.c \
	pbuf.c \
	raw.c \
	stats.c \
	sys.c \
	tcp.c \
	tcp_in.c \
	tcp_out.c \
	timeouts.c \
	udp.c \
	icmp.c \
	ip4_frag.c \
	ip4.c \
	ip4_addr.c \
	etharp.c \
	api_lib.c \
	api_msg.c \
	err.c \
	if_api.c \
	netbuf.c \
	netdb.c \
	netifapi.c \
	tcpip.c \
	ethernet.c
endif
