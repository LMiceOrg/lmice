/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef INCLUDE_WORKER_BOARD_DATA_H_
#define INCLUDE_WORKER_BOARD_DATA_H_

#include <stdint.h>

#include <unistd.h>

#include <array>

#include "eal/lmice_eal_common.h"
#include "eal/lmice_eal_rwlock.h"

#include "include/service_logging_data.h"
#include "include/trader_data.h"


namespace lmice {

enum lm_board_e { LM_BOARD_RUNNING = 0, LM_BOARD_QUIT = 1 };

enum lm_worker_e {
  LM_WORKER_STANDBY = 0,
  LM_WORKER_START = 1,
  LM_WORKER_WORKING = 2,
  LM_WORKER_STOP = 3,
  LM_WORKER_SIZE = 6,
  LM_SPI_WORKER = 0,
  LM_BULK_SIZE = 16,
  LM_BULK_END = LM_BULK_SIZE,
  LM_BULK_PADDING = 128,
  LM_FLAG_SIZE = 128

};

struct lm_worker_status {
  lmice_rwlock_t m_lock;
  volatile int32_t m_status; /*< worker status start, working stop */
  pid_t m_pid;               /*< worker process pid */
  char name[16];             /*< worker name */
};

struct lm_aligned_bulk {
  int m_pos;
  int m_size;
};

struct lm_worker_board_data {
  int m_pid;
  int m_signal;
  int m_status;      /*< running status of board */
  int m_worker_size; /*< worker size */
  int m_quote_size;
  int m_quote_depth;
  int m_contract_size;
  int m_feature_size;
  lmice_rwlock_t m_flag_lock;
  lmice_rwlock_t m_bulk_lock;

  std::array<int64_t, LM_FLAG_SIZE> m_flags;

  int64_t m_launch_time; /*< unit:100 nano second */
  uint64_t m_board_size; /*< total size in bytes of board*/

  uint64_t m_bulk_pos;   /*< next address of bulk */
  uint64_t m_bulk_count; /* existing bulks count */

  uint64_t m_flag_pos;
  uint64_t m_flag_count;

  uint64_t m_logging_pos;  /*< service logging current position */
  uint64_t m_logging_mask; /*< service logging data mask */

  std::array<lm_worker_status, LM_WORKER_SIZE> m_workers;
  std::array<lm_aligned_bulk, LM_BULK_SIZE> m_bulks;

  // ft_trader_xone_spi m_spi; /*< xone spi */
  // ft_trader_xone_spi_data m_spi_data;
  // ft_trader_xone_api_data m_api_data;

  lm_service_logging_data m_logging_data; /*< service logging data */

  char remain_data[8];
} forcepack(128);

}  // namespace lmice

#endif  // INCLUDE_WORKER_BOARD_DATA_H_
