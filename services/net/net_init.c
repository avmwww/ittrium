/**
 * lwIP bring-up for ittrium: tcpip core, loopback, optional TCP echo :7
 */
#include "net_init.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#if LWIP_NETCONN
#include "lwip/api.h"
#include "lwip/sys.h"
#endif

#include "ittrium.h"

#ifndef NET_ECHO_ENABLE
#define NET_ECHO_ENABLE  1
#endif

#ifndef NET_ECHO_PORT
#define NET_ECHO_PORT  7
#endif

#ifndef NET_ECHO_THREAD_PRIO
#define NET_ECHO_THREAD_PRIO  DEFAULT_THREAD_PRIO
#endif

#ifndef NET_ECHO_THREAD_STACK
#define NET_ECHO_THREAD_STACK  DEFAULT_THREAD_STACKSIZE
#endif

#if LWIP_NETCONN && NET_ECHO_ENABLE

static void net_echo_thread(void *arg)
{
  struct netconn *listener;
  struct netconn *client;
  err_t err;

  LWIP_UNUSED_ARG(arg);

  listener = netconn_new(NETCONN_TCP);
  if (listener == NULL) {
    for (;;) {
      dly_tsk(1000);
    }
  }

  err = netconn_bind(listener, IP_ADDR_ANY, NET_ECHO_PORT);
  if (err != ERR_OK) {
    netconn_delete(listener);
    for (;;) {
      dly_tsk(1000);
    }
  }

  err = netconn_listen(listener);
  if (err != ERR_OK) {
    netconn_delete(listener);
    for (;;) {
      dly_tsk(1000);
    }
  }

  for (;;) {
    err = netconn_accept(listener, &client);
    if (err != ERR_OK || client == NULL) {
      continue;
    }

    for (;;) {
      struct netbuf *buf;
      void *data;
      u16_t len;

      err = netconn_recv(client, &buf);
      if (err != ERR_OK) {
        break;
      }

      do {
        netbuf_data(buf, &data, &len);
        err = netconn_write(client, data, len, NETCONN_COPY);
        if (err != ERR_OK) {
          break;
        }
      } while (netbuf_next(buf) >= 0);

      netbuf_delete(buf);
      if (err != ERR_OK) {
        break;
      }
    }

    netconn_close(client);
    netconn_delete(client);
  }
}

static void net_start_echo(void)
{
  sys_thread_new("echo7", net_echo_thread, NULL,
                 NET_ECHO_THREAD_STACK, NET_ECHO_THREAD_PRIO);
}

#endif /* LWIP_NETCONN && NET_ECHO_ENABLE */

static void net_tcpip_init_done(void *arg)
{
  volatile int *done = (volatile int *)arg;

  /* Loopback netif is created by lwIP when LWIP_HAVE_LOOPIF=1.
   * Ensure it is up for local testing without a driver. */
#if LWIP_HAVE_LOOPIF
  /* loopif is brought up in netif_init(); nothing else required here */
  (void)netif_default;
#endif

  if (done != NULL) {
    *done = 1;
  }
}

void net_init(void)
{
  volatile int tcpip_ready = 0;

  tcpip_init(net_tcpip_init_done, (void *)&tcpip_ready);

  /* Wait until tcpip thread has finished init callback */
  while (!tcpip_ready) {
    dly_tsk(1);
  }

#if LWIP_NETCONN && NET_ECHO_ENABLE
  net_start_echo();
#endif
}
