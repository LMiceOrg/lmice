/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef SERVICE_INCLUDE_SERVER_H_
#define SERVICE_INCLUDE_SERVER_H_

#include <stdio.h>

#include "eal/lmice_eal_shm.h"
#include "include/uds.h"

/** struct and enum */
enum {
  LM_QUIT = 1,
  LM_CLIENT,
  LM_SERVER,
  LM_TRACE,
  LM_REGCLIENT,
  LM_UNREGCLIENT,
  LM_SUB,
  LM_UNSUB,
  LM_PUB,
  LM_UNPUB,
  LM_SENDDATA
};

/* lmice server data-structure */
struct lmice_server_s {
  int exec_type; /* exec type */
  int exec_flag; /* execution quit flag */
  int uid;
  int pid_fd;  /* file descriptor of the pid file */
  int sock_fd; /* socket for client communication */
  char pid_file[64];
  char sock_file[64];
  char config_file[64];
  lmice_shm_t shm; /* shm */
  lmmsg_t m_msg;   /* uds message */
};
typedef struct lmice_server_s server_t;

// typedef void* board_t;

/** function declaration */
int server_init(int argc, char** argv, server_t** srv);

board_t server_board(server_t* srv);

#endif /* SERVICE_INCLUDE_SERVER_H_ */
