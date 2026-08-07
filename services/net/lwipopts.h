/**
 * lwIP options for ittrium (μITRON)
 *
 * Keep modest MEMP sizes suitable for embedded targets.
 * Include path must contain this directory (or copy/symlink as lwipopts.h).
 */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

/* OS integration */
#define NO_SYS                      0
#define SYS_LIGHTWEIGHT_PROT        1
#define LWIP_TIMERS                 1

/* API selection: netconn yes, BSD sockets no */
#define LWIP_NETCONN                1
#define LWIP_SOCKET                 0
#define LWIP_NETCONN_SEM_PER_THREAD 0

/* Core protocols */
#define LWIP_IPV4                   1
#define LWIP_IPV6                   0
#define LWIP_DHCP                   0
#define LWIP_AUTOIP                 0
#define LWIP_DNS                    0
#define LWIP_UDP                    1
#define LWIP_TCP                    1
#define LWIP_ICMP                   1
#define LWIP_RAW                    0
#define LWIP_ARP                    0
#define LWIP_ETHERNET               0

/* Loopback */
#define LWIP_NETIF_LOOPBACK         1
#define LWIP_HAVE_LOOPIF            1
#define LWIP_LOOPBACK_MAX_PBUFS     4
#define LWIP_NETIF_LOOPBACK_MULTITHREADING 1

/* Checksums: software */
#define CHECKSUM_GEN_IP             1
#define CHECKSUM_GEN_UDP            1
#define CHECKSUM_GEN_TCP            1
#define CHECKSUM_GEN_ICMP           1
#define CHECKSUM_CHECK_IP           1
#define CHECKSUM_CHECK_UDP          1
#define CHECKSUM_CHECK_TCP          1
#define CHECKSUM_CHECK_ICMP         1

/* Memory / pools (modest) */
#define MEM_ALIGNMENT               8
#define MEM_SIZE                    (32 * 1024)

#define MEMP_NUM_PBUF               16
#define MEMP_NUM_UDP_PCB            4
#define MEMP_NUM_TCP_PCB            8
#define MEMP_NUM_TCP_PCB_LISTEN     2
#define MEMP_NUM_TCP_SEG            16
#define MEMP_NUM_REASSDATA          4
#define MEMP_NUM_FRAG_PBUF          4
#define MEMP_NUM_ARP_QUEUE          4
#define MEMP_NUM_IGMP_GROUP         4
#define MEMP_NUM_SYS_TIMEOUT        8
#define MEMP_NUM_NETBUF             8
#define MEMP_NUM_NETCONN            8
#define MEMP_NUM_TCPIP_MSG_API      8
#define MEMP_NUM_TCPIP_MSG_INPKT    8

#define PBUF_POOL_SIZE              16
#define PBUF_POOL_BUFSIZE           256

/* TCP tunables */
#define TCP_MSS                     256
#define TCP_SND_BUF                 (2 * TCP_MSS)
#define TCP_WND                     (4 * TCP_MSS)
#define TCP_SND_QUEUELEN            ((4 * TCP_SND_BUF) / TCP_MSS)
#define TCP_LISTEN_BACKLOG          1

/* Mailbox / thread sizes used by sys layer & tcpip */
#define TCPIP_MBOX_SIZE             8
#define DEFAULT_UDP_RECVMBOX_SIZE   4
#define DEFAULT_TCP_RECVMBOX_SIZE   4
#define DEFAULT_ACCEPTMBOX_SIZE     2
#define DEFAULT_RAW_RECVMBOX_SIZE   2

#define TCPIP_THREAD_STACKSIZE      2048
#define TCPIP_THREAD_PRIO           3
#define DEFAULT_THREAD_STACKSIZE    1536
#define DEFAULT_THREAD_PRIO         4

/* Stats / debug off by default */
#define LWIP_STATS                  0
#define LWIP_DEBUG                  0

/* errno from newlib */
#define LWIP_PROVIDE_ERRNO          0

#endif /* LWIPOPTS_H */
