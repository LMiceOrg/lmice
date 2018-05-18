#ifndef DISCRETE_OTHER_BEST_SINCE_H
#define DISCRETE_OTHER_BEST_SINCE_H

#include "iaca/iacaMarks.h"

#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef other_template
#define other_template fm_discrete_other_best_since<feature_type>
namespace lmice {

template <class feature_type>
struct fm_discrete_other_best_since
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
  typedef fm_discrete_other_best_since<feature_type> this_type;
  typedef fm_discrete_other_translation_feature<this_type, feature_type>
      base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;
  typedef fm_open_feature<feature_type> open_feature_type;

  // using msg_type::get_inst;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_other_best_since() {}

  inline void reset(void) { m_ema.reset(); }

  inline void init(int trading_fd, int reference_fd, float_type ratio_mics,
                   bool use_diff, bool corr_negative) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init other quote fd */
    set_other_quote(reference_fd);

    /** init private members */
    base_type* base = static_cast<base_type*>(this);
    base->init(ratio_mics, use_diff, corr_negative);
    reset();
    //    if (reference_fd == 2)
    //      printf("obs[%d] %lf, %d, %d\n", reference_fd, ratio_mics, use_diff,
    //             corr_negative);
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
    // printf("call o best\n");
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();

    float_type mid = (bid + offer) / 2.0;
    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    m_ema.update(this->calculatedRatio(mid, other_mid), time_micro);

    float_type translated_other_bid =
        this->translatedBid(cur_other_bid, cur_other_offer);
    float_type translated_other_offer =
        this->translatedOffer(cur_other_bid, cur_other_offer);
    float_type best_bid_since =
        translated_other_bid > bid ? (translated_other_bid - bid) : 0;
    float_type best_offer_since =
        translated_other_offer < offer ? (offer - translated_other_offer) : 0;

    set_signal((best_bid_since - best_offer_since) / mid);
  }
  inline void handle_other_msg() {
    //    unsigned high[2], low[2];
    //    eal_atomic_cycle_warmup(high, low);
    //    eal_atomic_cycle_begin(high, low);

    float_type other_bid = get_cur_other_bid();
    float_type other_offer = get_cur_other_offer();
    int64_t other_time_micro = get_cur_other_time_micro();

    float_type prev_bid = get_cur_bid();
    float_type prev_offer = get_cur_offer();
    //    if (prev_bid <= 0 || prev_offer <= prev_bid) {
    //      return;
    //    }

    float_type mid = (prev_bid + prev_offer) / 2.0;
    float_type other_mid = (other_bid + other_offer) / 2.0;

    //    static int id = 0;
    //    if (m_other_fd == 2) {
    //      if (id < 3) {
    //        printf("obs ema %.14lf\t%16.14lf, %lf, %lf\n", m_ema.get_value(),
    //               m_ema.get_event(), mid, other_mid);
    //      }
    //      ++id;
    //    }

    m_ema.update(this->calculatedRatio(mid, other_mid), other_time_micro);
    //    eal_atomic_cycle_end(high + 1, low + 1);

    //    static int i = 0;
    //    if (i <= 1) {
    //      uint64_t v = eal_atomic_cycle_value(high, low);

    //      printf("time count of obs other %llu\n", v);
    //      i++;
    //    }
  }
};
}  // namespace lmice

#endif  // DISCRETE_OTHER_BEST_SINCE_H
