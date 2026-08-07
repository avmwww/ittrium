/**
 * Network stack init for ittrium + lwIP
 */
#ifndef NET_INIT_H
#define NET_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise lwIP (tcpip thread), bring up loopback, and optionally
 * start a TCP echo server on port 7 (NET_ECHO_ENABLE).
 *
 * Call from a task context after the kernel is running (e.g. init task).
 */
void net_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_INIT_H */
