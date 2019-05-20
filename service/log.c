/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include "include/log.h"
enum { LM_DOUBLE_BUFFER_RING };
typedef int (*message_callback)(int fd, int nitems);
int create_log(board_t* brd);
/**
 * 创建内存环形容器；返回环形容器的描述符 descriptor
 */
int create_ring(board_t* brd, int item_size, int nitems);
int set_message_handler(int fd, int msg, message_callback* cb);

/** function declaration */
int log_write(int fd, int nitems);

int log_open(server_t* srv) {
  board_t board;
  int ring_fd;
  int log_fd;

  /* get board from server */
  board = server_board(srv);

  /* create log item */
  log_fd = create_log(board);

  /* create double buffer ring */
  ring_fd = create_ring(board, 1024, 16);

  /* Register handler callbacks */
  set_message_handler(ring_fd, LM_DOUBLE_BUFFER_RING, log_write);

  return log_fd;
}

int log_close(int fd) {}