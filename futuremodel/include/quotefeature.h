/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_
#define FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_

#include <array>

#include "include/featurebase.h"
namespace lmice {

template <class float_type, int quote_size>
struct fm_quote_feature {
  enum { MAX_REFERENCE_COUNT = 4 };
  typedef std::array<feature_range, MAX_REFERENCE_COUNT> ref_range_type;
  std::array<feature_range, quote_size> m_self_range;
  std::array<ref_range_type, quote_size> m_ref_range;
};
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_
