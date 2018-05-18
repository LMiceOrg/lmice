#ifndef TRADER_SHM_WORKER_H
#define TRADER_SHM_WORKER_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach/vm_statistics.h>
#endif

#include <stdint.h>

#include <array>

#include "eal/lmice_eal_rwlock.h"
#include "eal/lmice_eal_shm.h"
#include "eal/lmice_trace.h"

namespace lmice {
enum ft_shm_worker_e {
    LM_WORKER_STOP = 3,
    LM_WORKER_START = 1,
    LM_WORKER_WORKING = 2,
    LM_WORKER_STANDBY = 0,
    LM_BOARD_QUIT = 1,
    LM_BOARD_RUNNING=0
};

struct ft_worker_status {
    int32_t m_status; /* start, working stop */
    lmice_rwlock_t m_lock;
    pid_t m_pid;
};

template<int worker_size>
struct ft_worker_board {
    int m_status;  /*< running status of board */
    int m_worker_size; /* worker size */
    int m_quote_size;
    int m_quote_depth;
    int m_contract_size;
    int m_feature_size;


    std::array<ft_worker_status, worker_size> m_workers;

    static int create(lmice_shm_t* shm) {
        void* shm_addr;
        size_t shm_size = sizeof(ft_worker_board);
        shm_size = (shm_size +2*1024*1024-1)/(2*1024*1024) *(2*1024*1024);

        shm_addr =
            mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
      #if defined(__MACH__)
                 VM_FLAGS_SUPERPAGE_SIZE_2MB,
      #elif defined(__linux__)
                 MAP_HUGETLB,
      #endif
                 0);
        if (shm_addr == MAP_FAILED) {
            int err = errno;
          lmice_error_print(
              "Failed[%d] to create memory bulk by host super page size[%luMB] "
              "failed[%p]\n",
              err, shm_size / (1024 * 1024), shm_addr);
          shm_addr =
              mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
      #if defined(__MACH__)
                   SUPERPAGE_NONE,
      #elif defined(__linux__)
                   MAP_HUGETLB,
      #endif
                   0);
          if (shm_addr == MAP_FAILED) {
              int err = errno;
            lmice_error_print("Failed[%d] to create memory bulk by host size[%luMB]\n",
                              err, shm_size / (1024 * 1024));
            perror(NULL);
            return EXIT_FAILURE;
          }
        }
        memset(shm_addr, 0, shm_size);

        shm->addr = shm_addr;
        shm->size = shm_size;

        return EXIT_SUCCESS;

    }
    static void destroy(lmice_shm_t* shm)
    {
        munmap(shm->addr, shm->size);
    }
protected:
    ft_worker_board() = default;
};
}
#endif // TRADER_SHM_WORKER_H
