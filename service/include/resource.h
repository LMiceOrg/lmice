/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef SERVICE_INCLUDE_RESOURCE_H_
#define SERVICE_INCLUDE_RESOURCE_H_

/* C library */
#include <stdint.h>

/* eal library */
#include <eal/lmice_eal_common.h>
#include <eal/lmice_eal_thread.h>
#include "eal/lmice_eal_event.h"
#include "eal/lmice_eal_shm.h"
/*
# 资源

由资源描述符（整形）索引的，应用于交互计算的信息内容;系统以会话（session）分组管理资源，以发布订阅同步状态。
列表对象位于默认会话（0）
session         --会话 session（会话），=0 表示默认会话；>0表示其他会话
session-list    --会话列表
board           --内存资源，从os申请共享内存
board-list      --内存列表
resource-list   --资源列表，位于board首部，记录共享内存区域的分配使用
ring            --环形容器，从board申请一款连续内存区域
double-ring     --双缓冲环形容器，从board申请
client          --应用
client-list     --应用列表
stack-ring      --堆栈环

## 资源描述符

32比特定长整形
0-...-7:    索引会话
8-...-31:   索引资源


## 资源列表

系统最多拥有（2^8，256）个资源会话。
列表最多可以拥有（2^24， 1677,7216）个资源。
资源列表资源**存储**在系统首要资源列表当中；系统第一个资源列表即是系统的首要资源列表。
资源列表的扩展通过串接后续创建的资源列表而成。资源列表由系统内部维护。

## 计算机节点（node）

信息传输与事件委派；包含一个服务程序和多个应用程序。

## 应用 application

发布订阅资源与计算过程的集合

### 初始化 application_init

注册应用(session)

### 发布订阅资源

### 注册消息响应事件

## 平台 server (aka app0)

管理并维护共享资源

### 打开平台 server_create

初始化资源（创建资源列表 | 创建共享区域列表 | 创建会话列表  | 创建应用列表）
--> 初始化本节点(初始化网络通讯 | 发布节点资源 | 订阅节点资源) -->
初始化应用（注册应用(0) --》发布订阅资源 --》注册消息响应事件）

返回值 0 正常，其他 非正常

### 结束平台 server_destroy

关闭应用（注销响应事件 | 取消资源发布订阅）-->
关闭本地节点(取消资源发布订阅 | 结束网络通讯) -->
关闭资源 （关闭应用 | 关闭资源 | 关闭会话 | 关闭共享区域）

*/

int server_create();
int server_destroy();

int client_init(int session, void** client);

int create_session(int session);
int open_session(int session);
int destroy_session(int session);

/* 检查会话状态 */
int check_session(int session, int* status);

/* 返回资源列表 */

/* 双缓冲环形容器 */
int create_double_ring(int fd, int item_size, int nitems);

/* 环形容器 */
int create_ring(int fd, int item_size, int nitems);

/* 堆栈环 */
int create_stack_ring(int fd, int item_size, int nitems);

/* 事件 */
typedef int (*message_callback)(int fd, int msg, void* data);
int set_message_handler(int fd, int msg, message_callback, void* data);

/* macro */

#define BRDLIST_COUNT 8
#define SESLIST_COUNT 8
#define RESLIST_COUNT 64
#define APPLIST_COUNT 16
#define NODELIST_COUNT 16
#define LM_BULK_PADDING 128

enum res_e {
  RES_DEAD = 0,
  RES_ALIVE = 1,
  SYMBOL_LENGTH = 32,
  BOARD_LENGTH = 2048 * 1024,

  BOARD0_FD = 0,
  RESLIST_FD = 1,
  BRDLIST_FD = 2,
  SESLIST_FD,
  APPLIST_FD,
  NODELIST_FD
};

enum res_type_e {
  RESLIST_TP,
  RES_BOARD_T,
  BRDLIST_TP,
  SESLIST_TP,
  RES_SESSION_LIST_T,
  RES_RING_T,
  RES_DOUBLE_RING_T,
  RES_SESSION_T,

  APP_TP,
  APPLIST_TP,
  NODE_TP,
  NODELIST_TP

};

struct res_list_s {
  int32_t fd;              /* 描述符 */
  int32_t type;            /* 类型 */
  volatile long lock;      /* 自旋锁 */
  int32_t size;            /* 数据所占用内存大小 8字节整数倍 */
  int32_t count;           /* 列表数量 */
  struct res_list_s* next; /* 指向列表下一块 */
  char item[8];            /* 列表数据 */
};

typedef struct res_list_s res_list_t;

struct node_s {
  int32_t ip; /* ipv4 address */
};

typedef struct node_s node_t;

struct application_s {
  int32_t fd;   /* 描述符 */
  int32_t type; /* 资源类型 */
  eal_tid_t tid;
  volatile int flags; /* alive */
  pid_t pid;
  char name[SYMBOL_LENGTH];
  res_list_t* rlist; /* 资源列表 */
  res_list_t* blist; /* 共享区域列表 */
  res_list_t* slist; /* 会话列表 */
  res_list_t* nlist; /* 节点列表 */
  res_list_t* alist; /* 应用列表 */
};
typedef struct application_s app_t;

extern app_t* lmapp;

struct board_s {
  int fd;
  int size; /* total size */
  int pos;  /* current position */
  int flags;
  char* addr; /* start position */
};
typedef struct board_s board_t;

struct res_item_s {
  int32_t fd;    /* 描述符 */
  int32_t type;  /* 资源类型 */
  int32_t board; /* 内存块描述符 */
  int32_t pos;   /* 偏移量 */
  int32_t size;  /* 大小 */
  int32_t flags; /* alive */
};
typedef struct res_item_s res_item_t;

int rlist_find_or_create(void* shm_addr, int type, int size, int count,
                         void** item);

int rlist_find_type_first(res_list_t* rlist, int type, res_item_t** item);

struct session_s {
  int32_t size;
  int32_t type;

  int fd;
  int session;
  int flags;
  char name[32];
};
typedef struct session_s session_t;

#define LIST_SIZE (sizeof(res_list_t) - sizeof(char[8]))

#define RESLIST_SIZE_T (LIST_SIZE + sizeof(res_item_t) * RESLIST_COUNT)
#define BRDLIST_SIZE_T (LIST_SIZE + sizeof(board_t) * BRDLIST_COUNT)
#define SESLIST_SIZE_T (LIST_SIZE + sizeof(session_t) * SESLIST_COUNT)
#define APPLIST_SIZE_T (LIST_SIZE + sizeof(app_t) * APPLIST_COUNT)
#define NODELIST_SIZE_T (LIST_SIZE + sizeof(node_t) * NODELIST_COUNT)

#define PAD_SIZE(x) \
  (((x + LM_BULK_PADDING - 1) / LM_BULK_PADDING) * LM_BULK_PADDING)

#define RESLIST_SIZE PAD_SIZE(RESLIST_SIZE_T)

#define BRDLIST_SIZE PAD_SIZE(BRDLIST_SIZE_T)

#define SESLIST_SIZE PAD_SIZE(SESLIST_SIZE_T)

#define APPLIST_SIZE PAD_SIZE(APPLIST_SIZE_T)

#define NODELIST_SIZE PAD_SIZE(NODELIST_SIZE_T)

#define RESLIST_POS 0
#define BRDLIST_POS (RESLIST_POS + RESLIST_SIZE)
#define SESLIST_POS (BRDLIST_POS + BRDLIST_SIZE)
#define APPLIST_POS (SESLIST_POS + SESLIST_SIZE)
#define NODELIST_POS (APPLIST_POS + APPLIST_SIZE)

#endif /* SERVICE_INCLUDE_RESOURCE_H_ */
