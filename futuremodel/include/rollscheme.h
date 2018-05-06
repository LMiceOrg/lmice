/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_ROLLSCHEME_H_
#define FUTUREMODEL_INCLUDE_ROLLSCHEME_H_

#include <time.h>

#include <string>

#include "include/chinal1msg.h"

namespace lmice {
class RollScheme {
 public:
  static std::string from_alias(const std::string &alias, const tm &date);
  static void from_alias2(product_id &id, const std::string &alias,
                          const tm &date);
  static double get_ticksize(const std::string &product);
  static double get_constract_size(const std::string &product);
};

struct product_item {
  product_id id;
  double tick_size;
  double constract_size;
};
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_ROLLSCHEME_H_
