#ifndef DISCRETE_OTHER_BOOK_IMBALANCE_H
#define DISCRETE_OTHER_BOOK_IMBALANCE_H

#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef other_template
#define other_template fm_discrete_other_book_imbalance<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_other_book_imbalance
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
  typedef fm_discrete_other_book_imbalance<feature_type> this_type;
  typedef fm_discrete_other_translation_feature<this_type, feature_type>
      base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_other_book_imbalance() {}

  inline void reset(void) { this->m_ema.reset(); }

  inline void init(int trading_fd, int reference_fd, int tick_size,
                   float_type ratio_mics, bool use_diff, bool corr_negative) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init other quote fd */
    set_other_quote(reference_fd);

    /** init private members */
    m_tick_size = tick_size;
    base_type* base = static_cast<base_type*>(this);
    base->init(ratio_mics, use_diff, corr_negative);
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
    if (mid != 0 && other_mid != 0 && time_micro > 0) {
      float_type value = this->calculatedRatio(mid, other_mid);
      m_ema.init(value, time_micro);
      return true;
    }
    return false;
  }

  inline void handle_self_msg() {
    // printf("call o bi \n");
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();
    ;
    float_type cur_other_bid_quantity = get_cur_other_bid_quantity();
    float_type cur_other_offer_quantity = get_cur_other_offer_quantity();

    float_type mid = (bid + offer) / 2.0;
    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    this->m_ema.update(this->calculatedRatio(mid, other_mid), time_micro);

    float_type other_book_imbalance =
        (cur_other_bid_quantity - cur_other_offer_quantity) /
        (cur_other_bid_quantity + cur_other_offer_quantity);
    float_type other_emid = (cur_other_bid + cur_other_offer) / 2.0 +
                            other_book_imbalance * m_tick_size / 2.0;
    float_type translated_other_emid = this->translatedPrice(other_emid);
    float_type translated_other_tick =
        this->translatedDelta(m_tick_size, other_mid);
    float_type translated_other_emid_return = translated_other_emid / mid - 1.0;
    if (translated_other_emid_return > translated_other_tick / mid) {
      translated_other_emid_return = translated_other_tick / mid;
    } else if (translated_other_emid_return < -translated_other_tick / mid) {
      translated_other_emid_return = -translated_other_tick / mid;
    }

    float_type signal = this->signedSignal(other_book_imbalance);

    signal = signal * translated_other_emid_return;
    if (signal < 0) signal = 0;

    set_signal(signal);

    //    if (signal > 0 && translated_other_emid_return > 0) {
    //      signal = signal * translated_other_emid_return;
    //      set_signal(signal);
    //    } else if (signal < 0 && translated_other_emid_return < 0) {
    //      signal = -signal * translated_other_emid_return;
    //      set_signal(signal);
    //    }
  }
  inline void handle_other_msg() {
    float_type prev_bid = get_prev_bid();
    float_type prev_offer = get_prev_offer();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();
    int64_t cur_other_time_micro = get_cur_other_time_micro();

    float_type mid = (prev_bid + prev_offer) / 2.0;
    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    this->m_ema.update(this->calculatedRatio(mid, other_mid),
                       cur_other_time_micro);
  }
};
}  // namespace lmice

#endif  // DISCRETE_OTHER_BOOK_IMBALANCE_H
