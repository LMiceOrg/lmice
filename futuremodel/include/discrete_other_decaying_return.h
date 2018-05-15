#ifndef DISCRETE_OTHER_DECAYING_RETURN_H
#define DISCRETE_OTHER_DECAYING_RETURN_H

#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef other_template
#define other_template fm_discrete_other_decaying_return<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_other_decaying_return
    : public fm_other_feature<other_template>,
      public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_other_decaying_return<feature_type> this_type;
  typedef fm_other_feature<this_type> base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_other_decaying_return() {}

  inline void reset(void) { m_ema.reset(); }

  inline void init(int trading_fd, int reference_fd, float_type decay_mics) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init other quote fd */
    set_other_quote(reference_fd);

    /** init private members */
    m_ema.init(1.0 / decay_mics);
    reset();
  }

  inline bool prepare() {
    // prepare m_ema

    int64_t time_micro = get_cur_time_micro();

    float_type cur_other_bid = get_cur_other_bid();
    float_type cur_other_offer = get_cur_other_offer();

    float_type other_mid = (cur_other_bid + cur_other_offer) / 2.0;
    if (other_mid != 0 && time_micro > 0) {
      float_type value = other_mid;
      m_ema.init(value, time_micro);
      return true;
    }
    return false;
  }

  void handle_self_msg() {
    float_type other_bid = get_cur_other_bid();
    float_type other_offer = get_cur_other_offer();

    int64_t time_micro = get_cur_time_micro();
    double other_mid = (other_bid + other_offer) / 2.0;
    m_ema.update(other_mid, time_micro);
    float_type signal = (other_mid - m_ema.get_value()) / other_mid;
    set_signal(signal);
  }
  void handle_other_msg() {
    float_type other_bid = get_cur_other_bid();
    float_type other_offer = get_cur_other_offer();
    int64_t other_time_micro = get_cur_other_time_micro();

    double other_mid = (other_bid + other_offer) / 2.0;
    m_ema.update(other_mid, other_time_micro);
  }
};
}  // namespace lmice

#endif  // DISCRETE_OTHER_DECAYING_RETURN_H
