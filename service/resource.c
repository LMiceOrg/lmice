/** Copyright 2018 He Hao<hehaoslj@sina.com> */

/* C library */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

/* system library */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach/vm_statistics.h>
#include <sys/mman.h>
#elif defined(__linux__)
#include <sys/mman.h>
#endif

/* eal library */
#include "eal/lmice_eal_hash.h"
#include "eal/lmice_eal_shm.h"
#include "eal/lmice_eal_spinlock.h"
#include "eal/lmice_trace.h"

#include "include/resource.h"

/* macro */
#define SESSION_NAME "LMiced V1.1 Session 000"
#define BOARD_NAME "/private/tmp/LMiced V1.1 Board 000"

/* declaration */
app_t* lmapp = NULL;

/* function */

static int mkfd(int type, int pos);

static int resource_init();
static int resource_finit();

static int node_init();

static int board_init(int size, char** addr);

static void reslist_init(res_list_t* rlist, int fd, int tp, int size,
                         int count);

int server_create() {
  /* 初始化资源 */
  resource_init();
  /* 初始化本地节点 */
  node_init();
  /* 初始化 应用 */
  return 0;
}

int server_destroy() {
  resource_finit();
  return 0;
}

int resource_init() {
  int ret;
  char* addr = NULL;
  res_list_t* rlist;
  res_item_t* item;
  res_list_t* blist;
  res_list_t* slist;
  res_list_t* alist;
  res_list_t* nlist;
  board_t* board0;

  /* board0 init */
  ret = board_init(BOARD_LENGTH, &addr);
  if (ret != 0) {
    return ret;
  }
  lmice_info_print("board %p\n", addr);

  rlist = (res_list_t*)(addr + RESLIST_POS);

  /* 初始化资源列表 */
  reslist_init(rlist, RESLIST_FD, RESLIST_TP, RESLIST_SIZE, RESLIST_COUNT);

  /* 注册 资源列表 资源 */
  {
    item = (res_item_t*)(rlist->item);

    item->fd = RESLIST_FD;
    item->type = RESLIST_TP;
    item->board = BOARD0_FD;
    item->pos = RESLIST_POS;
    item->size = RESLIST_SIZE;
    item->flags = RES_ALIVE;
  }

  lmice_info_print("init resource list\n");

  /* 初始化共享区域列表 */
  blist = (res_list_t*)(addr + BRDLIST_POS);
  reslist_init(blist, BRDLIST_FD, BRDLIST_TP, BRDLIST_SIZE, BRDLIST_COUNT);
  /* 注册 共享区域列表 资源 */
  {
    item = (res_item_t*)(rlist->item) + 1;
    item->fd = BRDLIST_FD;
    item->type = BRDLIST_TP;
    item->board = BOARD0_FD;
    item->pos = BRDLIST_POS;
    item->size = BRDLIST_SIZE;
    item->flags = RES_ALIVE;
  }

  /* 注册 共享区域0 */
  {
    board_t* brd;
    /* board 0 */
    brd = (board_t*)blist->item;

    brd->addr = addr;
    brd->fd = BOARD0_FD;
    brd->size = BOARD_LENGTH;
    brd->pos = BRDLIST_POS + BRDLIST_SIZE;
    brd->flags = RES_ALIVE;

    board0 = brd;
  }

  /* 初始化会话列表 */
  slist = (res_list_t*)(addr + SESLIST_POS);
  reslist_init(slist, SESLIST_FD, SESLIST_TP, SESLIST_SIZE, SESLIST_COUNT);

  /* 注册 会话列表 资源 */
  {
    item = (res_item_t*)(rlist->item) + 2;
    /* session list */
    item->fd = SESLIST_FD;
    item->type = SESLIST_TP;
    item->board = BOARD0_FD;
    item->pos = SESLIST_POS;
    item->size = SESLIST_SIZE;
    item->flags = RES_ALIVE;
  }
  /* 注册 会话0 */
  {
    session_t* ses;
    /* session 0 */
    ses = (session_t*)slist->item;

    ses->flags = RES_ALIVE;
    ses->size = sizeof(session_t);
    ses->type = RES_SESSION_T;
    ses->session = 0;
  }

  /* 创建应用列表 */
  alist = (res_list_t*)(addr + APPLIST_POS);
  reslist_init(alist, APPLIST_FD, APPLIST_TP, APPLIST_SIZE, APPLIST_COUNT);

  /* 注册 应用列表 资源 */
  {
    item = (res_item_t*)(rlist->item) + 3;
    /* application list */
    item->fd = APPLIST_FD;
    item->type = APPLIST_TP;
    item->board = BOARD0_FD;
    item->pos = APPLIST_POS;
    item->size = APPLIST_SIZE;
    item->flags = RES_ALIVE;
  }

  /* 注册 应用0 */
  {
    app_t* app;
    app = (app_t*)(alist->item);
    app->fd = mkfd(APP_TP, 0);
    lmapp = app;
  }

  /* 创建节点列表 */
  nlist = (res_list_t*)(addr + NODELIST_POS);
  reslist_init(nlist, NODELIST_FD, NODELIST_TP, NODELIST_SIZE, NODELIST_COUNT);

  /* 注册 节点列表 资源 */
  {
    item = (res_item_t*)(rlist->item) + 3;
    /* node list */
    item->fd = NODELIST_FD;
    item->type = NODELIST_TP;
    item->board = BOARD0_FD;
    item->pos = NODELIST_POS;
    item->size = NODELIST_SIZE;
    item->flags = RES_ALIVE;
  }

  /* 注册 节点0 */
  {
    node_t* node;
    node = (node_t*)(nlist->item);
  }

  /* 更新共享区域0 */
  board0->pos = NODELIST_POS + NODELIST_SIZE;

  lmapp->alist = alist;
  lmapp->blist = blist;
  lmapp->nlist = nlist;
  lmapp->rlist = rlist;
  lmapp->slist = slist;

  return 0;
}

void reslist_init(res_list_t* rlist, int fd, int tp, int size, int count) {
  rlist->fd = fd;
  rlist->type = tp;
  rlist->lock = 0;
  rlist->size = size;
  rlist->count = count;
  rlist->next = 0;
}

int board_init(int size, char** addr) {
  void* host_bulk;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_ANON | MAP_SHARED;
#if defined(__MACH__)
  int fd = VM_FLAGS_SUPERPAGE_SIZE_ANY;
#elif defined(__linux__)
  int fd = MAP_HUGETLB;
#endif

  host_bulk = mmap(NULL, size, prot, flags, fd, 0);
  if (host_bulk == MAP_FAILED) {
    fd = -1;
    host_bulk = mmap(NULL, size, prot, flags, fd, 0);
  }
  if (host_bulk == MAP_FAILED) {
    return EXIT_FAILURE;
  }
  *addr = host_bulk;
  return EXIT_SUCCESS;
}

int mkfd(int tp, int pos) { return (tp << 24) + pos; }

int node_init() {
  node_t* node;
  node = (node_t*)(lmapp->nlist->item);
  node->ip = 0;

  return 0;
}

int resource_finit() {
  res_list_t* blist = lmapp->blist;
  board_t* board0 = NULL;
  do {
    int i;
    board0 = (board_t*)(blist->item);
    for (i = 1; i < blist->count; ++i) {
      board_t* board;
      board = (board_t*)(blist->item) + i;
      if (board->flags == RES_ALIVE) {
        munmap(board->addr, board->size);
      }
    }
    blist = blist->next;
  } while (blist != NULL);

  if (board0) {
    munmap(board0->addr, board0->size);
  }

  return 0;
}
