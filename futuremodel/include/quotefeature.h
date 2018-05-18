/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_
#define FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_

#include <array>
#include <functional>
#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"
namespace lmice {

template <class open_type>
struct fm_quote_feature {
 private:
  typedef typename fm_type_traits<open_type>::float_type float_type;
  enum {
    quote_size = fm_type_traits<open_type>::m_quote_size,
    contract_size = fm_type_traits<open_type>::m_contract_size,
    quote_depth = fm_type_traits<open_type>::m_quote_depth,
    feature_size = fm_type_traits<open_type>::m_feature_size
  };
  typedef fm_open_feature<open_type> open_feature_type;
  typedef fm_portfolio<float_type, quote_size, quote_depth, contract_size,
                       feature_size>
      portfolio_type;

 public:
  enum { MAX_REFERENCE_COUNT = 4 };
  typedef std::array<feature_range, MAX_REFERENCE_COUNT> ref_range_type;

  typedef void (*handle_function)(portfolio_type*);
  typedef std::array<handle_function, MAX_REFERENCE_COUNT> ref_function_type;

  std::array<feature_range, quote_size> m_self_range;
  std::array<ref_range_type, quote_size> m_ref_range;
  std::array<handle_function, quote_size> m_handle_self_msg;
  std::array<ref_function_type, quote_size> m_handle_other_msg;
};
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_QUOTEFEATURE_H_
