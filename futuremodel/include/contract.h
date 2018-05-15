#ifndef CONTRACT_H
#define CONTRACT_H

#include "include/chinal1msg.h"
#include "include/featurebase.h"

namespace lmice {

struct fm_order_msg {
  int id;
};
struct fm_cancel_msg {
  int id;
};

template <class float_type>
struct fm_contract {
  feature_range m_feature_range;  //合约开仓子信号
  float_type m_forecast;
  fm_order_msg m_order;    //交易报单
  fm_cancel_msg m_calcel;  //撤销交易报单
};
}  // namespace lmice
#endif  // CONTRACT_H
