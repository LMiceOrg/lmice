#ifndef DISCRETE_SELF_BOOK_IMBALANCE_H
#define DISCRETE_SELF_BOOK_IMBALANCE_H
#include "eal/lmice_eal_common.h"

#include "include/chinal1msg.h"
#include "include/featurebase.h"
#include "include/openfeature.h"

#undef self_template
#define self_template fm_discrete_self_book_imbalance<feature_type>
namespace lmice {

template <class feature_type>
class fm_discrete_self_book_imbalance : public fm_self_feature<self_template>,
                                        public fm_open_feature<feature_type> {
 public:
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth
  };
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_discrete_self_book_imbalance<feature_type> this_type;
  typedef fm_self_feature<this_type> base_type;
  typedef fm_feature_chinal1_handler<fm_open_feature<feature_type>,
                                     feature_type>
      msg_type;

  // friend msg_type;
  USING_FEATURE_METHODS()

  USING_FEATURE_DATA(fm_feature_data<feature_type>)

  fm_discrete_self_book_imbalance() {}

  inline void reset(void) {}

  inline void init(int trading_fd, float_type tick_size) {
    /** init self quote fd */
    set_self_quote(trading_fd);

    /** init private members */
    m_tick_size = tick_size;
    reset();
  }
  inline bool prepare() {
    float_type bid = get_cur_bid();
    if (bid > 0) return true;

    return false;
  }

  void handle_self_msg() {
    float_type bid = get_cur_bid();
    float_type bid_quantity = get_cur_bid_quantity();

    float_type offer = get_cur_offer();
    float_type offer_quantity = get_cur_offer_quantity();

    if (bid_quantity <= 0 || offer_quantity <= 0) {
      return;
    }

    float_type mid = 0.5 * (bid + offer);
    float_type imbalance =
        (bid_quantity - offer_quantity) / (bid_quantity + offer_quantity);
    set_signal(imbalance * m_tick_size / mid);
    //    static int i = 0;
    //    if (i < 2) {
    //      ++i;
    //      printf("sbib: %lf %lf %lf\n", m_signal, mid, m_signal);
    //    }
  }
};
}  // namespace lmice
#endif  // DISCRETE_SELF_BOOK_IMBALANCE_H
