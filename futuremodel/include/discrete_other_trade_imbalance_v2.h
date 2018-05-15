#ifndef DISCRETE_OTHER_TRADE_IMBALANCE_V2_H
#define DISCRETE_OTHER_TRADE_IMBALANCE_V2_H

#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef other_template
#define other_template fm_discrete_other_trade_imbalance_v2<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_other_trade_imbalance_v2
    : public fm_discrete_other_translation_feature<other_template,
                                                   feature_type>,
      public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_other_trade_imbalance_v2<feature_type> this_type;
  typedef fm_discrete_other_translation_feature<this_type, feature_type>
      base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_other_trade_imbalance_v2() {}

  inline void reset(void) {
    this->m_ema.reset();
    m_book.reset();
  }

  inline void init(int trading_fd, int reference_fd, int tick_size,
                   int contract_size, float_type ratio_mics, bool use_diff,
                   bool corr_negative, float_type book_decay) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init other quote fd */
    set_other_quote(reference_fd);

    /** init private members */
    m_tick_size = tick_size;
    m_contract_size = contract_size;
    base_type* base = static_cast<base_type*>(this);
    base->init(ratio_mics, use_diff, corr_negative);
    m_book.init(1.0 / book_decay);
    reset();
  }

  inline bool prepare() {
    // prepare m_ema
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();

    float_type mid = (bid + offer) / 2.0;
    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    if (bid > 0 && offer > 0 && cur_other_bid > 0 && cur_other_offer > 0 &&
        time_micro > 0) {
      float_type value = this->calculatedRatio(mid, other_mid);
      m_ema.init(value, time_micro);
    } else
      return false;

    // prepare m_book
    int other_bid_quantity = get_cur_other_bid_quantity();
    int other_offer_quantity = get_cur_other_offer_quantity();
    int64_t other_time_micro = get_cur_other_time_micro();

    if (other_time_micro > 0) {
      float_type value = other_bid_quantity + other_offer_quantity;
      m_book.update(value, other_time_micro);
    } else
      return false;

    return true;
  }

  void handle_self_msg() {
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();
    int cur_other_volume = get_cur_other_volume();
    float_type cur_other_notional = get_cur_other_notional();

    float_type prev_other_bid = get_prev_other_bid();
    float_type prev_other_offer = get_prev_other_offer();
    int prev_other_volume = get_prev_other_volume();
    float_type prev_other_notional = get_prev_other_notional();

    // float_type prev_bid = get_prev_bid();
    // float_type prev_offer = get_prev_offer();

    float_type mid = (bid + offer) / 2.0;
    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    this->m_ema.update(this->calculatedRatio(mid, other_mid), time_micro);

    if (cur_other_volume <= prev_other_volume) {
    } else {
      float_type other_vwap = (cur_other_notional - prev_other_notional) /
                              (cur_other_volume - prev_other_volume) /
                              m_contract_size;
      float_type other_prev_mid = (prev_other_bid + prev_other_offer) / 2.0;
      // float_type other_mid = (cur_other_bid + cur_other_offer) / 2.;

      float_type other_vwap_minus_prev_mid = other_vwap - other_prev_mid;
      if (other_vwap_minus_prev_mid > 0.5 * m_tick_size) {
        other_vwap_minus_prev_mid = 0.5 * m_tick_size;
      } else if (other_vwap_minus_prev_mid < -0.5 * m_tick_size) {
        other_vwap_minus_prev_mid = -0.5 * m_tick_size;
      }
      double other_trade_imbalance =
          other_vwap_minus_prev_mid * 2.0 / m_tick_size *
          fast_sqrt((cur_other_volume - prev_other_volume) /
                    m_book.get_value());

      float_type translated_other_vwap = this->translatedPrice(other_vwap);
      // float_type mid = (bid + offer) / 2.0;
      float_type translated_other_tick =
          this->translatedDelta(m_tick_size, other_mid);
      float_type translated_other_vwap_return =
          translated_other_vwap / mid - 1.0;

      if (translated_other_vwap_return > translated_other_tick / mid) {
        translated_other_vwap_return = translated_other_tick / mid;
      } else if (translated_other_vwap_return < -translated_other_tick / mid) {
        translated_other_vwap_return = -translated_other_tick / mid;
      }
      float_type signal = this->signedSignal(other_trade_imbalance);

      if (signal > 0 && translated_other_vwap_return > 0) {
        signal = signal * translated_other_vwap_return;
      } else if (signal < 0 && translated_other_vwap_return < 0) {
        signal = -signal * translated_other_vwap_return;
      } else {
        signal = 0;
      }
      set_signal(signal);
    }
  }
  void handle_other_msg() {
    int other_bid_quantity = get_cur_other_bid_quantity();
    int other_offer_quantity = get_cur_other_offer_quantity();
    int64_t other_time_micro = get_cur_other_time_micro();

    m_book.update(other_bid_quantity + other_offer_quantity, other_time_micro);
    float_type other_bid = get_cur_other_bid();
    float_type other_offer = get_cur_other_offer();

    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    //
    double mid = (bid + offer) / 2.0;
    double other_mid = (other_bid + other_offer) / 2.0;
    this->m_ema.update(this->calculatedRatio(mid, other_mid), time_micro);
  }
};
}  // namespace lmice

#endif  // DISCRETE_OTHER_TRADE_IMBALANCE_V2_H
