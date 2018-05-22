#ifndef INCLUDE_SERVICE_LOGGING_DATA_H_
#define INCLUDE_SERVICE_LOGGING_DATA_H_

#include <array>

#include <stdint.h>

namespace lmice {
enum { LOGGING_SIZE = 32, LOGGING_CTX = 512 };
enum {
  LMICE_TRACE_INFO = 0,
  LMICE_TRACE_DEBUG = 1,
  LMICE_TRACE_WARNING = 2,
  LMICE_TRACE_ERROR = 3,
  LMICE_TRACE_CRITICAL = 4,
  LMICE_TRACE_NONE = 5,
  LMICE_TRACE_TIME
};

struct lm_service_logging_ctx {
  int m_type;
  int m_pid;
  int64_t m_time;
  char m_mesg[LOGGING_CTX - 16];
};

struct lm_service_logging_data {
  std::array<lm_service_logging_ctx, LOGGING_SIZE> m_ctxes;
};

}  // namespace lmice

#endif  // INCLUDE_SERVICE_LOGGING_DATA_H_
