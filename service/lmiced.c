/** Copyright 2018 He Hao<hehaoslj@sina.com>
 *
 * 资源列表 ：资源描述符：整形索引，资源描述，资源内容
 */

/** c library */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/** system */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__MACH__) || defined(__linux__)
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif

/** jansson */
#include <jansson.h>

/**  eal */
#include "eal/lmice_eal_hash.h"
#include "eal/lmice_eal_shm.h"
#include "eal/lmice_trace.h"

#include "include/execution_manager.h"
#include "include/resource.h"
#include "include/server.h"
#include "include/uds.h"

/** macro */
#define PID_FILE "/var/run/lmiced.pid"
#define SOCK_FILE "/var/run/lmiced.socket"
#define CONFIG_FILE "/etc/lmiced.conf"
#define BOARD_NAME "LMiced V1.1"
#define BOARD_SIZE 2 * 1024 * 1024

/** function declaration */
// static void signal_handler(int sig);
// static int daemon_init(void);

// static void server_usage(void);
// static int server_init(server_t* server, int argc, char* argv[]);
// static int server_launch(server_t* server);
// static int server_poll(server_t* server);
// static void server_proc(server_t* server, lmmsg_t* msg);
// static void server_quit(server_t* server);
// static void server_uninit(server_t* server);

/** Global publish/subscribe client list */

int main(int argc, char* argv[]) {
  int ret;
  server_t* server = NULL;
  (void)argc;
  (void)argv;

  lmice_info_print("Lmice daemon starting...\n");

  /* Init server */
  ret = server_create();
  // ret = server_init(server, argc, argv);
  if (ret != 0) return ret;
  {
    board_t* brd = (board_t*)(lmapp->blist->item);
    lmice_warning_print("board0 size %d  pos %d\n", brd->size, brd->pos);
  }

  // if (server->m_exec_type == LM_CLIENT) {
  //   execution_init();
  //   return 0;
  // }

  // /* Exclussive launch: group & other can read it */
  // ret = server_launch(server);
  // if (ret != 0) return ret;

  // /* poll client event */
  // server_poll(server);

  // /* clean resources */
  lmice_info_print("Lmice destroying...\n");
  server_destroy(server);

  return 0;
}

// void signal_handler(int sig) {
//   if (sig == SIGTERM) {
//     g_server.m_exec_flag = LM_QUIT;
//   }
// }

// int daemon_init(void) {
//   int no_chgdir = 0;
//   int no_chgstdio = 0;
//   int ret = 0;
//   if (daemon(no_chgdir, no_chgstdio) != 0) {
//     ret = errno;
//   }
//   return ret;
// }

// int server_init(server_t* server, int argc, char** argv) {
//   int ret = 0;
//   int i;
//   size_t len;

//   /* reset server data struct */
//   memset(server, 0, sizeof(server_t));
//   server->m_uid = getuid();
//   memcpy(server->m_pid_file, PID_FILE, sizeof(PID_FILE));
//   memcpy(server->m_sock_file, SOCK_FILE, sizeof(SOCK_FILE));
//   memcpy(server->m_config_file, CONFIG_FILE, sizeof(CONFIG_FILE));
//   server->m_exec_type = LM_SERVER;
//   /* process commandline */
//   for (i = 0; i < argc; ++i) {
//     char* cmd = argv[i];
//     if (str_equal(cmd, "-h") || str_equal(cmd, "--help")) {
//       server_usage();
//       return 1;
//     } else if (str_equal(cmd, "-d") || str_equal(cmd, "--device")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         len = strlen(cmd);
//         if (len > 63) {
//           lmice_error_print("Adapter device name is too long(>63)\n");
//           return 1;
//         }
//         memcpy(server->m_device_name, cmd, len);
//       } else {
//         lmice_error_print("Command(device) require device name\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "-u") || str_equal(cmd, "--uid")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         server->m_uid = str_int(cmd, 0, strlen(cmd));
//         if (ret != 0) {
//           ret = errno;
//           lmice_error_print("Change to user(%d) failed %d.\n", server->m_uid,
//                             ret);
//           return ret;
//         }
//       } else {
//         lmice_error_print("Command(uid) require device name\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "-p") || str_equal(cmd, "--pid")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         len = strlen(cmd);
//         if (len > sizeof(server->m_pid_file) - 1) {
//           lmice_error_print("Command(pid) name is too long(>%lu)\n", len);
//           return 1;
//         }
//         memcpy(server->m_pid_file, cmd, len + 1);

//       } else {
//         lmice_error_print("Command(pid) require file name\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "-s") || str_equal(cmd, "--socket")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         len = strlen(cmd);
//         if (len > sizeof(server->m_sock_file) - 1) {
//           lmice_error_print("Command(socket) name is too long(>%lu)\n", len);
//           return 1;
//         }
//         memcpy(server->m_sock_file, cmd, len + 1);

//       } else {
//         lmice_error_print("Command(socket) require file name\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "--port")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         len = strlen(cmd);
//         server->m_port = str_int(cmd, 0, len);
//       } else {
//         lmice_error_print("Command(port) require port number\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "-c") || str_equal(cmd, "--config")) {
//       if (i + 1 < argc) {
//         cmd = argv[i + 1];
//         len = strlen(cmd);
//         if (len > sizeof(server->m_config_file) - 1) {
//           lmice_error_print("Command(config) name is too long(>%lu)\n", len);
//           return 1;
//         }
//         memcpy(server->m_config_file, cmd, len + 1);

//       } else {
//         lmice_error_print("Command(config) require file name\n");
//         return 1;
//       }
//       ++i;
//     } else if (str_equal(cmd, "--client")) {
//       server->m_exec_type = LM_CLIENT;
//     } else if (str_equal(cmd, "--server")) {
//       server->m_exec_type = LM_SERVER;
//     }
//   }

//   /* config file */

//   return ret;
// }
// int server_launch(server_t* server) {
//   int ret = 1;

//   lmice_info_print("Lmice server launching...\n");

//   /* Set mask of process, others & group without exec right */
//   umask(S_IXGRP | S_IXOTH);

//   /* Hugepage memory */
//   eal_shm_zero(&server->m_shm);
//   uint64_t hval = eal_hash64_fnv1a(BOARD_NAME, sizeof(BOARD_NAME) - 1);
//   eal_shm_hash_name(hval, server->m_shm.name);
//   server->m_shm.size = BOARD_SIZE; /* multiple 4K bytes*/
//   ret = eal_shm_create(&server->m_shm);
//   if (ret != 0) {
//     lmice_error_print("Shm board init failed\n");
//     return ret;
//   }
//   memset(server->m_shm.addr, 0, server->m_shm.size);

//   /* unix domain socket */
//   ret = udss_init(server->m_sock_file, &server->m_msg);

//   server->m_pid_fd = open(server->m_pid_file, O_CREAT | O_RDWR, 0644);
//   if (server->m_pid_fd > 0) {
//     struct flock lock;
//     ssize_t sz;
//     ssize_t len;
//     lock.l_type = F_WRLCK;
//     lock.l_whence = SEEK_SET;
//     lock.l_start = 0;
//     lock.l_len = 0;
//     if (fcntl(server->m_pid_fd, F_SETLK, &lock) == 0) {
//       eal_pid_t pid;
//       char buf[32] = {0};
//       pid = getpid();
//       len = snprintf(buf, sizeof(buf), "%d\n", pid);
//       ++len;
//       sz = write(server->m_pid_fd, buf, len);
//       if (sz == len) {
//         ret = 0;
//       }
//     }
//   }

//   if (ret != 0) {
//     lmice_error_print("Lmice server launch failed\n");
//     return ret;
//   }

//   /* Init Daemon */
//   ret = daemon_init();
//   if (ret != 0) {
//     lmice_error_print("Daemon init failed [%d]\n", ret);
//     return ret;
//   }

//   /* set uid */
//   setuid(server->m_uid);

//   /* Listen and wait signal to end */
//   signal(SIGTERM, signal_handler);

//   return 0;
// }

// int server_poll(server_t* server) {
//   int ret;
//   struct pollfd fds[1];
//   lmmsg_t msg;

//   /* init msg */
//   memset(&msg, 0, sizeof(msg));
//   msg.m_addr_len = sizeof(msg.m_remote);

//   /* init pollfd */
//   fds[0].fd = server->m_sock_fd;
//   fds[0].events = POLLIN;
//   fds[0].revents = 0;

//   while (1) {
//     if (server->m_exec_flag == LM_QUIT) {
//       break;
//     }
//     /* wait 100 milli-seconds */
//     ret = poll(fds, 1, 100);

//     if (ret < 0) {
//       /* error occured */
//       break;
//     } else if (ret == 0) {
//       /* time out */
//       continue;
//     }

//     /* recv msg data */
//     msg.m_size = recvfrom(server->m_sock_fd, msg.m_data, sizeof(msg.m_data),
//     0,
//                           (struct sockaddr*)&(msg.m_remote),
//                           &msg.m_addr_len);
//     /* proc msg */
//     if (msg.m_size > 4) server_proc(server, &msg);

//   } /* while-1 */

//   return 0;
// }

// void server_proc(server_t* server, lmmsg_t* msg) {
//   int msg_type;

//   msg_type = (int)(*msg->m_data);
//   switch (msg_type) {
//     case LM_QUIT:
//       server_quit(server);
//       break;
//     case LM_TRACE:
//       break;
//   } /* switch-msg_type */
// }

// void server_quit(server_t* server) { server->m_exec_flag = LM_QUIT; }

// void server_uninit(server_t* server) {
//   /* shared memory */
//   eal_shm_destroy(&server->m_shm);
//   /* client list */
//   /* pub/sub list */
//   /* socket */
//   close(server->m_sock_fd);
//   /* server pid file */
//   unlink(server->m_pid_file);
// }

// void server_usage(void) {
//   printf("LMiced - Lmice Message Interactive Computational Environment\n");
// }
