/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach/vm_statistics.h>
#endif

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eal/lmice_trace.h"

#include "include/portfolio.h"

enum {
  m_quote_size = 6,
  m_quote_depth = 2,
  m_contract_size = 1,
  m_feature_size = 32,
  m_worker_size = 2
};
typedef double float_type;

// 投资组合
using lmice::fm_portfolio;
typedef fm_portfolio<float_type, m_quote_size, m_quote_depth, m_contract_size,
                     m_feature_size>
    pf_type;
typedef typename lmice::fm_open_feature_type<float_type, m_quote_size,
                                             m_quote_depth, m_contract_size,
                                             m_feature_size>::type open_type;
typedef lmice::fm_open_feature<open_type> open_feature_type;

typedef lmice::fm_discrete_other_best_since<open_type> obs_type;
typedef lmice::fm_discrete_other_book_imbalance<open_type> obi_type;
typedef lmice::fm_discrete_other_decaying_return<open_type> odr_type;
typedef lmice::fm_discrete_other_trade_imbalance<open_type> oti_type;
typedef lmice::fm_discrete_other_trade_imbalance_v2<open_type> oti2_type;

typedef lmice::fm_discrete_self_book_imbalance<open_type> sbi_type;
typedef lmice::fm_discrete_self_decaying_return<open_type> sdr_type;
typedef lmice::fm_discrete_self_orderflow_imbalance<open_type> soi_type;
typedef lmice::fm_discrete_self_trade_imbalance<open_type> sti_type;
typedef lmice::fm_discrete_self_trade_imbalance_v2<open_type> sti2_type;

template <class this_type>
static void handle_self_msg(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->handle_self_msg();
}
template <class this_type>
static void handle_other_msg(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->handle_other_msg();
}
template <class this_type>
static void prepare(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->prepare();
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  printf("sizeof portfolio %zu\n", sizeof(pf_type));

  // 创建共享存储区域
  size_t shm_size = 2 * 1024 * 1024;
  void* shm_addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_PRIVATE |
#if defined(__MACH__)
                            VM_FLAGS_SUPERPAGE_SIZE_2MB,
#elif defined(__linux__)
                            MAP_HUGETLB,
#endif
                        -1, 0);
  if (shm_addr == MAP_FAILED) {
    lmice_error_print(
        "Create memory bulk by host super page size[%luMB] "
        "failed[%p]\n",
        shm_size / (1024 * 1024), shm_addr);
    shm_addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE |
#if defined(__MACH__)
                        SUPERPAGE_NONE,
#elif defined(__linux__)
                        MAP_HUGETLB,
#endif
                    -1, 0);
    if (shm_addr == MAP_FAILED) {
      lmice_error_print("Create memory bulk by host size[%luMB] failed[%p]\n",
                        shm_size / (1024 * 1024), shm_addr);
      perror(NULL);
      return EXIT_FAILURE;
    }
  }
  memset(shm_addr, 0, shm_size);

  pf_type* portfolio = new (shm_addr) pf_type;
  int feature_fd;
  for (feature_fd = 0; feature_fd < m_feature_size; ++feature_fd) {
    portfolio->get_feature(feature_fd).init_portfolio(portfolio, feature_fd);
  }

  // portfolio = 0;
  printf("portfolio.%zu\n", portfolio->m_signals.size());
  munmap(shm_addr, shm_size);
  return 0;
}
