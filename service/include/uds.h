#ifndef INCLUDE_SERVICE_UDS_H_
#define INCLUDE_SERVICE_UDS_H_

/** c library */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/** system */
//#if defined(__MACH__) || defined(__linux__)
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
//#endif

struct lmice_uds_msg_s {
  int m_sock;
  socklen_t m_addr_len;
  struct sockaddr_un m_remote;
  struct sockaddr_un m_local;
  ssize_t m_size;
  char m_data[1024];
};
typedef struct lmice_uds_msg_s lmmsg_t;

int udss_init(const char* srv_name, lmmsg_t* msg);
int udsc_init(const char* srv_name, lmmsg_t* msg);
int uds_sendmsg(lmmsg_t* msg);
int udss_uninit(lmmsg_t* msg);
int udsc_uninit(lmmsg_t* msg);

#endif  // INCLUDE_SERVICE_UDS_H_
