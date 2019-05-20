/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef SERVICE_INCLUDE_RES_MANAGER_H_
#define SERVICE_INCLUDE_RES_MANAGER_H_

#include <stdint.h>

#define BLOCK_NAME_MAX 64
struct block_ops {
  void* (*create)(const char* name, int size);
  void* (*open)(const char* name);
  int (*close)(void* addr);
};

struct block {
  /* user-side name */
  char u_name[BLOCK_NAME_MAX];
  /* system-side name */
  char s_name[32];
  int fd;
  int size;
  char* addr;
  struct block_ops* op;
};

struct res_index {
  int32_t fd;
};
struct block_list {};

struct block_inode {
  int fd;
};

#endif  // SERVICE_INCLUDE_RES_MANAGER_H_
