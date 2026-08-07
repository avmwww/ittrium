################################################################################
include ../Makefile.rules

SERVICES  := $(KERNEL_DIR)/services
LWIPDIR   := $(KERNEL_DIR)/../lwip/src
LITTLEFS  := $(KERNEL_DIR)/third_party/littlefs

CFLAGS += -I$(SERVICES)/include
CFLAGS += -I$(SERVICES)/net
CFLAGS += -I$(SERVICES)/net/arch
CFLAGS += -I$(KERNEL_DIR)/kernel
CFLAGS += -I$(LWIPDIR)/include
CFLAGS += -I$(LITTLEFS)
CFLAGS += -DLWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS

vpath %.c src
vpath %.c $(SERVICES)/vfs
vpath %.c $(SERVICES)/shell
vpath %.c $(SERVICES)/proc
vpath %.c $(SERVICES)/elf
vpath %.c $(SERVICES)/net
vpath %.c $(SERVICES)/net/arch
vpath %.c $(LITTLEFS)
vpath %.c $(LWIPDIR)/core
vpath %.c $(LWIPDIR)/core/ipv4
vpath %.c $(LWIPDIR)/api
vpath %.c $(LWIPDIR)/netif

# --- application ---
SRCS = \
	test.c \
	uart.c \
	syscalls.c \
	string.c \

# --- services ---
SRCS += \
	vfs.c \
	romfs.c \
	ramfs.c \
	shell.c \
	lineedit.c \
	procfs.c \
	sysfs.c \
	elf_load.c \
	sys_arch.c \
	net_init.c \

# littlefs (lfs_port.c + lfs.c): hangs in lfs_mount — use ramfs for /data

# --- lwIP (NO_SYS=0, IPv4, netconn, loopback) ---
SRCS += \
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
	api_lib.c \
	api_msg.c \
	err.c \
	if_api.c \
	netbuf.c \
	netdb.c \
	netifapi.c \
	tcpip.c \

sub: $(LISTDIR) $(OBJS)

include Makefile.dep
