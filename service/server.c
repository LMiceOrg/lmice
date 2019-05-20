/** Copyright 2018 He Hao<hehaoslj@sina.com> */

/** c library */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/** eal lib */
#include "eal/lmice_trace.h"

#include "include/server.h"
#include "include/utils.h"

/** macro */
#define PID_FILE "/var/run/lmiced.pid"
#define SOCK_FILE "/var/run/lmiced.socket"
#define CONFIG_FILE "/etc/lmiced.conf"
#define BOARD_NAME "LMiced V1.1"
#define BOARD_SIZE 2 * 1024 * 1024

/** function declaration */
static void server_usage(void);

int server_init(int argc, char** argv, server_t** srv) {
  int ret;
  int i;
  size_t len;
  int uid;
  const char* pid_file = NULL;
  const char* sock_file = NULL;
  const char* config_file = NULL;

  for (i = 0; i < argc; ++i) {
    const char* cmd = argv[i];
    if (str_equal(cmd, "-h") || str_equal(cmd, "--help")) {
      server_usage();
      return 1;
    } else if (str_equal(cmd, "-u") || str_equal(cmd, "--uid")) {
      if (i + 1 < argc) {
        cmd = argv[i + 1];
        uid = str_int(cmd, 0, strlen(cmd));
        if (ret != 0) {
          ret = errno;
          lmice_error_print("Change to user id failed %d.\n", ret);
          return ret;
        }
      } else {
        lmice_error_print("Command(uid) require user id\n");
        return 1;
      }
      ++i;
    } else if (str_equal(cmd, "-p") || str_equal(cmd, "--pid")) {
      if (i + 1 < argc) {
        cmd = argv[i + 1];
        len = strlen(cmd);
        if (len > sizeof((*srv)->pid_file) - 1) {
          lmice_error_print("Command(pid) name is too long(>%lu)\n", len);
          return 1;
        }
        pid_file = cmd;

      } else {
        lmice_error_print("Command(pid) require file name\n");
        return 1;
      }
      ++i;
    } else if (str_equal(cmd, "-s") || str_equal(cmd, "--socket")) {
      if (i + 1 < argc) {
        cmd = argv[i + 1];
        len = strlen(cmd);
        if (len > sizeof((*srv)->sock_file) - 1) {
          lmice_error_print("Command(socket) name is too long(>%lu)\n", len);
          return 1;
        }
        sock_file = cmd;

      } else {
        lmice_error_print("Command(socket) require file name\n");
        return 1;
      }
      ++i;
    } else if (str_equal(cmd, "-c") || str_equal(cmd, "--config")) {
      if (i + 1 < argc) {
        cmd = argv[i + 1];
        len = strlen(cmd);
        if (len > sizeof((*srv)->config_file) - 1) {
          lmice_error_print("Command(config) name is too long(>%lu)\n", len);
          return 1;
        }
        config_file = cmd;

      } else {
        lmice_error_print("Command(config) require file name\n");
        return 1;
      }
      ++i;
    }
  }

  *srv = NULL;

  /* process params */

  /* create shared memory block */

  return 0;
}

void server_usage(void) {
  printf(
      "LMiced - Lmice Message Interactive Computational Environment\n"
      "  -- Copyright 2018 He Hao<hehaoslj@sina.com>\n");
}
