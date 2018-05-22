#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "eal/lmice_eal_atomic.h"
#include "eal/lmice_eal_time.h"

#include "include/service_logging.h"
#include "include/worker_board.h"

typedef lmice::lm_worker_board board_type;

extern board_type* g_board;
extern "C" void lmice_trace(int pid, int type, int64_t time,
                            const char* thread_name, const char* ctx);
namespace lmice {

static void log_data(int type, const char* format, va_list va) {
  uint64_t i;
  lm_service_logging_data* pdata;

  i = eal_atomic_xadd(&(g_board->m_logging_pos), 1);
  i = i % LOGGING_SIZE;

  pdata = &(g_board->m_logging_data);
  auto& buffer = pdata->m_ctxes[i];

  buffer.m_type = type;
  buffer.m_pid = getpid();
  get_system_time(&buffer.m_time);
  buffer.m_time -= g_board->m_launch_time;

  vsnprintf(buffer.m_mesg, sizeof(buffer.m_mesg), format, va);

  lmice_trace(buffer.m_pid, buffer.m_type, buffer.m_time, "UN", buffer.m_mesg);

  g_board->m_logging_mask |= (1 << i);
}

void lm_service_logging::lmice_info_printva(const char* format, ...) {
  va_list va;

  va_start(va, format);

  log_data(LMICE_TRACE_INFO, format, va);
  va_end(va);
}

void lm_service_logging::lmice_warning_printva(const char* format, ...) {
  va_list va;

  va_start(va, format);
  log_data(LMICE_TRACE_WARNING, format, va);
  va_end(va);
}

void lm_service_logging::lmice_critical_printva(const char* format, ...) {
  va_list va;

  va_start(va, format);
  log_data(LMICE_TRACE_CRITICAL, format, va);
  va_end(va);
}

void lm_service_logging::lmice_error_printva(const char* format, ...) {
  va_list va;

  va_start(va, format);
  log_data(LMICE_TRACE_ERROR, format, va);
  va_end(va);
}

void lm_service_logging::lmice_debug_printva(const char* format, ...) {
  va_list va;

  va_start(va, format);
  log_data(LMICE_TRACE_DEBUG, format, va);
  va_end(va);
}

void lm_service_logging::proc_msg() {
  uint64_t mask = g_board->m_logging_mask;
  uint64_t pos = g_board->m_logging_pos % LOGGING_SIZE;
  size_t i;
  for (i = pos; i < LOGGING_SIZE + pos; ++i) {
    size_t cur_pos = i % LOGGING_SIZE;
    if (mask & (1 << cur_pos)) {
      const auto& ctx = g_board->m_logging_data.m_ctxes[cur_pos];
      printf("%d %s\n", ctx.m_pid, ctx.m_mesg);
    }
  }

  eal_fetch_and_xor64(&g_board->m_logging_mask, mask);
}

}  // namespace lmice
