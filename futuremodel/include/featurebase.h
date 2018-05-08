/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_FEATUREBASE_H_
#define FUTUREMODEL_INCLUDE_FEATUREBASE_H_

#include <stdint.h>
#include <string>

#include "eal/lmice_eal_hash.h"
#include "eal/lmice_eal_shm.h"

#include "include/chinal1msg.h"
#include "include/fastmath.h"

namespace lmice {

template <class sub_class>
struct fm_check_type;

template <template <typename> class sub_class, typename fp_type>
struct fm_check_type<sub_class<fp_type>> {
  typedef fp_type float_type;
};

template <class sub_class>
class fm_feature_base {
 public:
  typedef typename fm_check_type<sub_class>::float_type float_type;
  fm_feature_base() {}
  explicit fm_feature_base(const std::string& feautre_name) {
    init(feautre_name);
  }
  inline void init(const std::string& feautre_name) {
    m_feature_id = eal_hash64_fnv1a(feautre_name.c_str(), feautre_name.size());
    m_signal = 0;
  }

  std::string get_feature_name() const {
    char name[32] = {0};
    eal_shm_hash_name(m_feature_id, name);
    return name;
  }

  float_type m_signal;

 private:
  uint64_t m_feature_id;
};

template <class sub_class>
class fm_discrete_self_feature {
 public:
  typedef typename fm_check_type<sub_class>::float_type float_type;
  typedef china_l1msg<float_type> china_l1msg;
  fm_discrete_self_feature() {}
  explicit fm_discrete_self_feature(const product_id& oid) { init(oid); }
  explicit fm_discrete_self_feature(const std::string& oid) {
    prod_string(m_trading_instrument, oid.c_str());
  }
  inline void init(const product_id& oid) {
    prod_copy(&m_trading_instrument, oid);
  }
  void handle_other_msg(const china_l1msg& msg) { (void)msg; }
  const product_id& tradingInstrument() const { return m_trading_instrument; }
  product_id& tradingInstrument() { return m_trading_instrument; }

 private:
  product_id m_trading_instrument;
};

template <class sub_class>
class fm_discrete_other_feature {
 public:
  typedef typename fm_check_type<sub_class>::float_type float_type;
  typedef china_l1msg<float_type> china_l1msg;

  fm_discrete_other_feature() {}

  fm_discrete_other_feature(const std::string& trading_instrument,
                            const std::string& reference_instrument) {
    prod_string(&m_trading_instrument, trading_instrument.c_str());
    prod_string(&m_reference_instrument, reference_instrument.c_str());
  }

  inline void init(const product_id& tid, const product_id& rid) {
    prod_copy(&m_trading_instrument, tid);
    prod_copy(&m_reference_instrument, rid);
  }

  std::string get_reference_instrument() const {
    return m_reference_instrument;
  }

  product_id m_trading_instrument;
  product_id m_reference_instrument;

 private:
};

template <class sub_class>
struct fm_discrete_other_translation_feature
    : public fm_feature_base<fm_discrete_other_translation_feature<sub_class>>,
      public fm_discrete_other_feature<
          fm_discrete_other_translation_feature<sub_class>> {
  typedef typename fm_check_type<sub_class>::float_type float_type;
  typedef china_l1msg<float_type> china_l1msg;
  typedef fm_ema<float_type> fast_ema;
  typedef fm_feature_base<fm_discrete_other_translation_feature> base_type;
  typedef fm_discrete_other_feature<fm_discrete_other_translation_feature>
      other_type;

  fm_discrete_other_translation_feature() {}

  fm_discrete_other_translation_feature(const std::string& feature_name,
                                        const std::string& trading_instrument,
                                        const std::string& reference_instrument,
                                        float_type ratio_mics, bool use_diff,
                                        bool corr_negative) {
    product_id tid;
    product_id rid;
    prod_string(&tid, trading_instrument.c_str());
    prod_string(&rid, reference_instrument.c_str());
    init(feature_name, tid, rid, ratio_mics, use_diff, corr_negative);
  }

  inline void init(const std::string& feature_name, const product_id& tid,
                   const product_id& rid, float_type ratio_mics, bool use_diff,
                   bool corr_negative) {
    base_type* base = static_cast<base_type*>(this);
    base->init(feature_name);

    other_type* other = static_cast<other_type*>(this);
    other->init(tid, rid);
    m_ema.init(1.0 / ratio_mics);
    m_use_diff = use_diff;
    m_corr_negative = corr_negative;
  }

  inline void init_other_msg(const china_l1msg* cur_other_msg,
                             const china_l1msg* prev_other_msg) {
    sub_class* t = static_cast<sub_class*>(this);
    t->init_other_msg(cur_other_msg, prev_other_msg);
  }
  inline void init_self_msg(const china_l1msg* cur_msg,
                            const china_l1msg* prev_msg) {
    sub_class* t = static_cast<sub_class*>(this);
    t->init_self_msg(cur_msg, prev_msg);
  }

  void handleOtherMsg(const china_l1msg& msg) {
    sub_class* t = static_cast<sub_class*>(this);
    t->handleOtherMsg(msg);
  }
  void handleSelfMsg(const china_l1msg& msg) {
    sub_class* t = static_cast<sub_class*>(this);
    t->handleSelfMsg(msg);
  }

  float_type calculatedRatio(float_type mid, float_type other_mid) const {
    return m_corr_negative
               ? (m_use_diff ? (mid + other_mid) : (mid * other_mid))
               : (m_use_diff ? (other_mid - mid) : (other_mid / mid));
  }
  float_type translatedPrice(float_type other_price) const {
    return m_corr_negative ? (m_use_diff ? (m_ema.get_value() - other_price)
                                         : (m_ema.get_value() / other_price))
                           : (m_use_diff ? (other_price - m_ema.get_value())
                                         : (other_price / m_ema.get_value()));
  }

  float_type translatedDelta(float_type other_delta,
                             float_type other_mid) const {
    return m_use_diff
               ? other_delta
               : (m_corr_negative
                      ? other_delta * m_ema.get_value() / other_mid / other_mid
                      : other_delta / m_ema.get_value());
  }

  float_type translatedBid(float_type other_bid, float_type other_offer) const {
    return m_corr_negative ? translatedPrice(other_offer)
                           : translatedPrice(other_bid);
  }

  float_type translatedOffer(float_type other_bid,
                             float_type other_offer) const {
    return m_corr_negative ? translatedPrice(other_bid)
                           : translatedPrice(other_offer);
  }

  float_type signedSignal(float_type signal) const {
    return m_corr_negative ? (-signal) : signal;
  }

  fast_ema m_ema;

 private:
  bool m_use_diff;
  bool m_corr_negative;
};

}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_FEATUREBASE_H_
