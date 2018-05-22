#include <stdint.h>

#include "include/service_logging_data.h"

namespace lmice {

struct lm_service_logging {
  static void lmice_info_printva(const char* format, ...);
  static void lmice_warning_printva(const char* format, ...);
  static void lmice_critical_printva(const char* format, ...);
  static void lmice_error_printva(const char* format, ...);
  static void lmice_debug_printva(const char* format, ...);

  void proc_msg();
};

}  // namespace lmice

#define lmice_info_print lmice::lm_service_logging::lmice_info_printva
#define lmice_warning_print lmice::lm_service_logging::lmice_warning_printva
#define lmice_critical_print lmice::lm_service_logging::lmice_critical_printva
#define lmice_error_print lmice::lm_service_logging::lmice_error_printva
#define lmice_debug_print lmice::lm_service_logging::lmice_debug_printva
