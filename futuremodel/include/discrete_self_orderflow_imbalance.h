/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef DISCRETE_SELF_ORDERFLOW_IMBALANCE_H
#define DISCRETE_SELF_ORDERFLOW_IMBALANCE_H
#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef self_template
#define self_template fm_discrete_self_orderflow_imbalance<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_self_orderflow_imbalance
    : public fm_self_feature<self_template>,
      public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_self_orderflow_imbalance<feature_type> this_type;
  typedef fm_self_feature<this_type> base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_self_orderflow_imbalance() {}

  inline void init(int trading_fd) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init private members */
    reset();
  }

  inline bool prepare() {
    float_type bid = get_cur_bid();
    if (bid > 0) return true;
    return false;
  }

  void handle_self_msg() {
    float_type bid = get_cur_bid();
    int bid_quantity = get_cur_bid_quantity();

    float_type offer = get_cur_offer();
    int offer_quantity = get_cur_offer_quantity();

    float_type prev_bid = get_prev_bid();
    float_type prev_offer = get_prev_offer();
    int prev_bid_qty = get_prev_bid_quantity();
    int prev_offer_qty = get_prev_offer_quantity();

    if (bid_quantity > 0 && offer_quantity > 0) {
      float_type bid_diff = bid - prev_bid;
      float_type bidflow_delta =
          bid_diff == 0 ? bid_quantity - prev_bid_qty : 0;

      float_type offer_diff = prev_offer - offer;
      float_type offerflow_delta =
          offer_diff == 0 ? offer_quantity - prev_offer_qty : 0;

      set_signal(bidflow_delta - offerflow_delta);
    }
  }

  inline void reset(void) {}
};
}  // namespace lmice

#endif  // DISCRETE_SELF_ORDERFLOW_IMBALANCE_H
