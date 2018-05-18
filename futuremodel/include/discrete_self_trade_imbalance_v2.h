/** Copyright 2018 He Hao<hehaoslj@sina.com> */
#ifndef FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_
#define FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_

#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef self_template
#define self_template fm_discrete_self_trade_imbalance_v2<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_self_trade_imbalance_v2
    : public fm_self_feature<self_template>,
      public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_self_trade_imbalance_v2<feature_type> this_type;
  typedef fm_self_feature<this_type> base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_self_trade_imbalance_v2() {}

  inline void init(int trading_fd, int contract_size, int tick_size,
                   float_type book_decay) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init private members */
    m_contract_size = contract_size;
    m_tick_size = tick_size;
    m_book.init(1.0 / book_decay);
    //    printf("init self tib2 %lf   %lf %lf\n", book_decay, m_tick_size,
    //           m_contract_size);
    reset();
  }

  inline bool prepare() {
    float_type bid = get_cur_bid();
    float_type prev_bid = get_prev_bid();
    float_type time_micro = get_cur_time_micro();
    if (bid > 0 && prev_bid > 0 && time_micro > 0) {
      float_type value = get_cur_bid_quantity() + get_cur_offer_quantity();
      m_book.init(value, time_micro);
      return true;
    }
    return false;
  }

  void reset() {
    // base_type* feature = static_cast<base_type*>(this);
    m_book.reset();
  }
  inline void handle_self_msg() {
    // sanity check the quotes
    m_book.update(get_cur_bid_quantity() + get_cur_offer_quantity(),
                  get_cur_time_micro());

    double bid = get_cur_bid();
    double offer = get_cur_offer();
    double volume = get_cur_volume();
    double notional = get_cur_notional();

    double prev_bid = get_prev_bid();
    double prev_offer = get_prev_offer();
    double prev_volume = get_prev_volume();
    double prev_notional = get_prev_notional();
    // double time = msg.get_time();

    if (volume <= prev_volume) {
    } else {
      double vwap =
          (notional - prev_notional) / (volume - prev_volume) / m_contract_size;
      double prev_mid = (prev_bid + prev_offer) / 2.;

      double mid = (bid + offer) / 2.;
      double vwap_minus_prev_mid = vwap - prev_mid;
      if (vwap_minus_prev_mid > 0.5 * m_tick_size) {
        vwap_minus_prev_mid = 0.5 * m_tick_size;
      } else if (vwap_minus_prev_mid < -0.5 * m_tick_size) {
        vwap_minus_prev_mid = -0.5 * m_tick_size;
      }

      double trade_imbalance =
          vwap_minus_prev_mid * 2.0 / m_tick_size *
          fast_sqrt((volume - prev_volume) / m_book.get_value());
      double vwap_minus_mid = vwap - mid;
      if (vwap_minus_mid > m_tick_size) {
        vwap_minus_mid = m_tick_size;
      } else if (vwap_minus_mid < -m_tick_size) {
        vwap_minus_mid = -m_tick_size;
      }

      if (vwap_minus_mid * trade_imbalance > 0) {
        float_type signal = fast_abs(vwap_minus_mid) / mid * trade_imbalance;
        set_signal(signal);
      }
    }
  }

};  // namespace lmice

}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_
