#ifndef DISCRETE_SELF_DECAYING_RETURN_H
#define DISCRETE_SELF_DECAYING_RETURN_H
#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef self_template
#define self_template fm_discrete_self_decaying_return<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_self_decaying_return : public fm_self_feature<self_template>,
                                         public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_self_decaying_return<feature_type> this_type;
  typedef fm_self_feature<this_type> base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_self_decaying_return() {}

  inline void reset(void) { m_ema.reset(); }

  inline void init(int trading_fd, float_type decay_mics) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init private members */
    m_ema.init(1.0 / decay_mics);
    reset();
  }

  inline bool prepare() {
    // prepare m_ema
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();
    int64_t time_micro = get_cur_time_micro();

    float_type mid = (bid + offer) / 2.0;

    if (bid > 0 && offer > 0 && time_micro > 0) {
      float_type value = mid;
      m_ema.init(value, time_micro);
      return true;
    }
    return false;
  }

  inline void handle_self_msg() {
    float_type bid = get_cur_bid();
    float_type offer = get_cur_offer();

    float_type mid = 0.5 * (bid + offer);
    m_ema.update(mid, get_cur_time_micro());
    float_type signal = (m_ema.get_value() - mid) / mid;
    set_signal(signal);
  }
};
}  // namespace lmice

#endif  // DISCRETE_SELF_DECAYING_RETURN_H
