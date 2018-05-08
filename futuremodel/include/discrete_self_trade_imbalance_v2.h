/** Copyright 2018 He Hao<hehaoslj@sina.com> */
#ifndef FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_
#define FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_

#include <string>

#include "eal/lmice_eal_common.h"

#include "include/featurebase.h"
#include "include/rollscheme.h"

namespace lmice {
template <class tp>
class fm_discrete_self_trade_imbalance_v2
    : public fm_feature_base<fm_discrete_self_trade_imbalance_v2<tp>>,
      public fm_discrete_self_feature<fm_discrete_self_trade_imbalance_v2<tp>> {
 public:
  typedef tp float_type;
  typedef china_l1msg<float_type> china_l1msg;
  typedef fm_ema<float_type> fast_ema;
  typedef fm_discrete_self_trade_imbalance_v2<tp> this_type;
  typedef fm_discrete_self_feature<fm_discrete_self_trade_imbalance_v2>
      self_type;
  typedef fm_feature_base<fm_discrete_self_trade_imbalance_v2> base_type;

  fm_discrete_self_trade_imbalance_v2() {}

  fm_discrete_self_trade_imbalance_v2(const std::string& feature_name,

                                      const std::string& trading_alias,
                                      const struct tm& date,
                                      float_type book_decay)
      : fm_feature_base<this_type>(feature_name),
        fm_discrete_self_feature<this_type>(
            roll_scheme::from_alias(trading_alias, date)),

        m_contract_size(roll_scheme::get_constract_size(trading_alias)),
        m_tick_size(roll_scheme::get_ticksize(trading_alias)),
        m_book(1.0 / book_decay) {
    reset();
  }

  inline void init(const std::string& feature_name,
                   const std::string& trading_alias, const struct tm& date,
                   float_type book_decay) {
    /** trading alias to trading id */
    product_id tid;
    std::string sid = roll_scheme::from_alias(trading_alias, date);
    prod_string(&tid, sid.c_str());

    /** init base */
    base_type* base = static_cast<base_type*>(this);
    base->init(feature_name);

    /** init self */
    self_type* self = static_cast<self_type*>(this);
    self->init(tid);

    /** init private members */
    m_contract_size = roll_scheme::get_constract_size(trading_alias);
    m_tick_size = roll_scheme::get_ticksize(trading_alias);
    m_book.init(1.0 / book_decay);
    reset();
  }

  inline void init_other_msg(const china_l1msg* cur_other_msg,
                             const china_l1msg* prev_other_msg) {
    lmice_unreferenced_param2(cur_other_msg, prev_other_msg);
  }
  inline void init_self_msg(const china_l1msg* cur_msg,
                            const china_l1msg* prev_msg) {
    lmice_unreferenced_param2(cur_msg, prev_msg);
  }

  void reset() {
    base_type* feature = static_cast<base_type*>(this);
    feature->m_signal = 0;
    m_book.reset();
  }
  void handle_self_msg(const china_l1msg& cur_msg, const china_l1msg& pre_msg) {
    base_type* feature = static_cast<base_type*>(this);

    // sanity check the quotes
    m_book.update(cur_msg.get_bid_quantity() + cur_msg.get_offer_quantity(),
                  cur_msg.get_time());
    double bid = cur_msg.get_bid();
    //  if (msg.get_bid_quantity() <= 0) {
    //    bid = 0;
    //  }

    double offer = cur_msg.get_offer();
    //  if (msg.get_offer_quantity() <= 0) {
    //    offer = 0;
    //  }

    double volume = cur_msg.get_volume();
    double notional = cur_msg.get_notional();

    double prev_bid = pre_msg.get_bid();
    double prev_offer = pre_msg.get_offer();
    double prev_volume = pre_msg.get_volume();
    double prev_notional = pre_msg.get_notional();
    // double time = msg.get_time();

    /*  if (prev_bid <= 0 || prev_offer <= prev_bid || bid <= 0 || offer <= bid)
      { m_signal = 0; } else */
    if (volume <= prev_volume) {
      feature->m_signal = 0;
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
        feature->m_signal = fast_abs(vwap_minus_mid) / mid * trade_imbalance;

      } else {
        feature->m_signal = 0;
      }
    }
  }

 private:
  float_type m_contract_size;
  float_type m_tick_size;
  fast_ema m_book;
};

}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_DISCRETE_SELF_TRADE_IMBALANCE_V2_H_
