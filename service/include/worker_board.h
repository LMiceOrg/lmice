#ifndef WORKER_BOARD_H
#define WORKER_BOARD_H

#include "eal/lmice_eal_shm.h"

#include "include/worker_board_data.h"

namespace lmice {
typedef int fd_t;

struct lm_worker_board : public lm_worker_board_data {
  static int create(lmice_shm_t* shm);
  static void destroy(lmice_shm_t* shm);

  fd_t create_bulk(int size);
  void* get_bulk(fd_t fd);
};
}  // namespace lmice
#endif  // WORKER_BOARD_H
