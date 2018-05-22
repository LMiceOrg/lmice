#ifndef WORKER_BOARD_H
#define WORKER_BOARD_H

#include "eal/lmice_eal_shm.h"

#include "include/worker_board_data.h"

namespace lmice {
struct lm_worker_board : public lm_worker_board_data {
  static int create(lmice_shm_t* shm);
  static void destroy(lmice_shm_t* shm);

  int create_bulk(int size);
  void* get_bulk(int pos);
};
}  // namespace lmice
#endif  // WORKER_BOARD_H
