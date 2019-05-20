#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach/vm_statistics.h>
#endif

#include <errno.h>

/** eal */
#include "eal/lmice_eal_time.h"

#include "include/service_logging.h"
#include "include/worker_board.h"

namespace lmice {

int lm_worker_board::create(lmice_shm_t* shm) {
  void* shm_addr;
  size_t shm_size = sizeof(lm_worker_board);
  shm_size =
      (shm_size + 2 * 1024 * 1024 - 1) / (2 * 1024 * 1024) * (2 * 1024 * 1024);

  shm_addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_SHARED |
#if defined(__MACH__)
                      VM_FLAGS_SUPERPAGE_SIZE_2MB,
#elif defined(__linux__)
                      MAP_HUGETLB,
#endif
                  -1, 0);
  if (shm_addr == MAP_FAILED) {
    int err = errno;
    lmice_error_print(
        "Failed[%d] to create HUGETLB bulk by host super page size[%luMB] "
        "failed[%p]\n",
        err, shm_size / (1024 * 1024), shm_addr);
    shm_addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_SHARED |
#if defined(__MACH__)
                        SUPERPAGE_NONE,
#elif defined(__linux__)
                        MAP_HUGETLB,
#endif
                    -1, 0);
    if (shm_addr == MAP_FAILED) {
      int err = errno;
      lmice_error_print(
          "Failed[%d] to create memory bulk by host size[%luMB]\n", err,
          shm_size / (1024 * 1024));
      perror(NULL);
      return EXIT_FAILURE;
    }
  }
  // printf("%p, sizeof %zu\n", shm_addr, sizeof(ft_worker_board));
  memset(shm_addr, 0, shm_size);

  lm_worker_board* pb = reinterpret_cast<lm_worker_board*>(shm_addr);
  pb->m_board_size = shm_size - sizeof(lm_worker_board);
  // assign pid
  pb->m_pid = getpid();
  // get launch time
  get_system_time(&pb->m_launch_time);
  // construct spi
  // new (&pb->m_spi) ft_trader_xone_spi;
  // init spi data
  //  pb->m_spi.set_spi_data(&pb->m_spi_data, &pb->m_api_data, &pb->m_pid,
  //                         &pb->m_signal);

  shm->addr = shm_addr;
  shm->size = shm_size;

  return EXIT_SUCCESS;
}

void lm_worker_board::destroy(lmice_shm_t* shm) {
  munmap(shm->addr, shm->size);
}

fd_t lm_worker_board::create_bulk(int size) {
  fd_t ret = LM_BULK_END;
  int refined_size =
      ((size + LM_BULK_PADDING - 1) / LM_BULK_PADDING) * LM_BULK_PADDING;

  if (this->m_bulk_count < this->m_bulks.size() &&
      this->m_bulk_pos + refined_size < this->m_board_size) {
    ret = this->m_bulk_count++;
    auto& bulk = this->m_bulks[ret];

    bulk.m_pos = this->m_bulk_pos;
    bulk.m_size = refined_size;
    this->m_bulk_pos += bulk.m_size;
  }
  return ret;
}

void* lm_worker_board::get_bulk(fd_t fd) {
  if (fd < LM_BULK_END && fd >= 0)
    return this->remain_data + this->m_bulks[fd].m_pos;
  else
    return nullptr;
}

}  // namespace lmice
