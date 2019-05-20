/** c lib */
#include <string.h>

/** system lib */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "include/uds.h"

static int make_socket_non_blocking(int sfd) {
  int flags, s;

  flags = fcntl(sfd, F_GETFL, 0);
  if (flags == -1) {
    return -1;
  }

  flags |= O_NONBLOCK;
  s = fcntl(sfd, F_SETFL, flags);
  if (s == -1) {
    return -1;
  }

  return 0;
}

int udss_init(const char* srv_name, lmmsg_t* msg) {
  int ret;

  memset(msg, 0, sizeof(lmmsg_t));
  msg->m_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (msg->m_sock == -1) {
    return -1;
  }

  msg->m_local.sun_family = AF_UNIX;
  strncpy(msg->m_local.sun_path, srv_name, strlen(srv_name));
  msg->m_local.sun_len = SUN_LEN(&msg->m_local);

  unlink(srv_name);

  ret = bind(msg->m_sock, (struct sockaddr*)&(msg->m_local),
             sizeof(msg->m_local));
  if (ret == -1) {
    return -1;
  }

  chmod(srv_name, 0666);

  ret = make_socket_non_blocking(msg->m_sock);

  if (ret == -1) return -1;

  return 0;
}

int udsc_init(const char* srv_name, lmmsg_t* msg) {
  int fd;
  char cli_name[64] = {0};
  sprintf(cli_name, "/run/user/%d/lmiced_cli_XXXXXX", geteuid());
  fd = mkstemp(cli_name);
  if (fd == -1) {
    return errno;
  }

  memset(msg, 0, sizeof(lmmsg_t));

  msg->m_remote.sun_family = AF_UNIX;
  strncpy(msg->m_remote.sun_path, srv_name, strlen(srv_name));
  msg->m_remote.sun_len = SUN_LEN(&msg->m_remote);

  if ((msg->m_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    // lmice_error_log("UDS client socket create failed.");
    return -1;
  }

  msg->m_local.sun_family = AF_UNIX;
  strncpy(msg->m_local.sun_path, cli_name, strlen(cli_name));
  msg->m_local.sun_len = SUN_LEN(&msg->m_local);
  close(fd);
  unlink(cli_name);

  if (bind(msg->m_sock, (struct sockaddr*)&(msg->m_local),
           sizeof(msg->m_local)) == -1) {
    // lmice_error_log("UDS client socket bind failed.");
    return -1;
  }

  /*
  if( make_socket_non_blocking(msg->sock) == -1) {
      lmice_error_log("UDS client socket nonblock failed.");
      return -1;
  }
  */
  return 0;
}

int uds_sendmsg(lmmsg_t* msg) {
  int ret;
  ret = sendto(msg->m_sock, msg->m_data, msg->m_size, 0,
               (struct sockaddr*)&(msg->m_remote), sizeof msg->m_remote);
  if (ret == msg->m_size) {
    ret = 0;
  }
  return ret;
}
