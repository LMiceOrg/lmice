/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_FEATUREBASE_H_
#define FUTUREMODEL_INCLUDE_FEATUREBASE_H_

#include <stdint.h>
#include <string>
#include <type_traits>

#include "eal/lmice_eal_hash.h"
#include "eal/lmice_eal_shm.h"

#include "include/chinal1msg.h"
#include "include/fastmath.h"

namespace lmice {

/*! ************ Macros ********/

#define GET_FEATURE_SELF2_S(name, type)                              \
  inline const type& get_##name() const {                            \
    const sub_class* t = static_cast<const sub_class*>(this);        \
    return t->m_portfolio->m_quotes.m_##name[t->m_self_fd].m_##name; \
  }                                                                  \
  inline const type& get_cur_##name() const { return get_##name(); } \
  inline const type& get_prev_##name() const { return get_##name(); }

#define GET_FEATURE_SELF2_M(name, type)                                       \
  template <int depth_pos = 0>                                                \
  inline const type& get_##name() const {                                     \
    const sub_class* t = static_cast<const sub_class*>(this);                 \
    return t->m_portfolio->m_quotes.m_msgs[t->m_self_fd][depth_pos].m_##name; \
  }                                                                           \
                                                                              \
  inline const type& get_cur_##name() const { return get_##name<0>(); }       \
                                                                              \
  inline const type& get_prev_##name() const { return get_##name<1>(); }

#define GET_FEATURE_OTHER2_S(name, type)                              \
  inline const type& get_other_##name() const {                       \
    const sub_class* t = static_cast<const sub_class*>(this);         \
    return t->m_portfolio->m_quotes.m_##name[t->m_other_fd].m_##name; \
  }                                                                   \
  inline const type& get_cur_other_##name() const {                   \
    return get_other_##name();                                        \
  }                                                                   \
  inline const type& get_prev_other_##name() const {                  \
    return get_other_##name();                                        \
  }

#define GET_FEATURE_OTHER2_M(name, type)                                       \
  template <int depth_pos = 0>                                                 \
  inline const type& get_other_##name() const {                                \
    const sub_class* t = static_cast<const sub_class*>(this);                  \
    return t->m_portfolio->m_quotes.m_msgs[t->m_other_fd][depth_pos].m_##name; \
  }                                                                            \
                                                                               \
  inline const type& get_cur_other_##name() const {                            \
    return get_other_##name<0>();                                              \
  }                                                                            \
                                                                               \
  inline const type& get_prev_other_##name() const {                           \
    return get_other_##name<1>();                                              \
  }

#define USING_FEATURE_DATA(data_type) \
  using data_type::m_type;            \
  using data_type::m_self_fd;         \
  using data_type::m_other_fd;        \
  using data_type::m_contract_size;   \
  using data_type::m_tick_size;       \
  using data_type::m_book;            \
  using data_type::m_ema;             \
  using data_type::m_use_diff;        \
  using data_type::m_corr_negative;   \
  using data_type::m_portfolio;

/*! *********predefined fm_portfolio ***************/
template <class float_type, int quote_size, int quote_depth, int contract_size,
          int feature_size>
struct fm_portfolio;

struct feature_range {
  int32_t m_feature_begin;  //第一个子信号
  int32_t m_feature_end;    //最后一个子信号
};

template <class feature_type>
struct fm_feature_data {
 private:
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  enum {
    m_quote_size = fm_type_traits<feature_type>::m_quote_size,
    m_quote_depth = fm_type_traits<feature_type>::m_quote_depth,
    m_contractmodel_size = fm_type_traits<feature_type>::m_contract_size,
    m_feature_size = fm_type_traits<feature_type>::m_feature_size
  };
  typedef fm_chinal1_msg<float_type, m_quote_size, m_quote_depth> quotes_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_portfolio<float_type, m_quote_size, m_quote_depth,
                       m_contractmodel_size, m_feature_size>
      portfolio_type;

 public:
  // uint64_t m_feature_id;  // feature_base
  // contract
  // product_id m_trading_instrument;    // self_feature
  // product_id m_reference_instrument;  // other feature

  bool m_use_diff;
  bool m_corr_negative;

  int m_type;  // feature type
  // int m_translation_type;

  /*! for quote */
  int m_self_fd;
  int m_other_fd;

  /*! for feature */
  int m_feature_fd;

  // float_type m_signal;

  float_type m_contract_size;
  float_type m_tick_size;

  fast_ema m_book;
  fast_ema m_ema;

  // const quotes_type* m_quotes;
  portfolio_type* m_portfolio;
};

template <class sub_class, class feature_type>
struct fm_feature_chinal1_handler {
 private:
  typedef typename fm_type_traits<feature_type>::float_type float_type;
  enum {
    m_quote_size = feature_type::m_quote_size,
    m_quote_depth = feature_type::m_quote_depth
  };
  typedef fm_chinal1_msg<float_type, m_quote_size, m_quote_depth> quotes_type;

  typedef sub_class T;

 public:
  inline void set_self_quote(int qfd) {
    static_cast<sub_class*>(this)->m_self_fd = qfd;
  }
  inline void set_other_quote(int qfd) {
    static_cast<sub_class*>(this)->m_other_fd = qfd;
  }

  inline void set_signal(float_type signal) {
    sub_class* t = static_cast<sub_class*>(this);
    t->m_portfolio->m_signals[t->m_feature_fd] = signal;
  }

  inline const float_type& get_signal() const {
    const sub_class* t = static_cast<const sub_class*>(this);
    return t->m_portfolio->m_signals[t->m_feature_fd];
  }

  GET_FEATURE_SELF2_S(inst, product_id)
  GET_FEATURE_SELF2_S(limit_up, float_type)
  GET_FEATURE_SELF2_S(limit_down, float_type)

  GET_FEATURE_SELF2_M(time_micro, int64_t)
  GET_FEATURE_SELF2_M(bid, float_type)
  GET_FEATURE_SELF2_M(offer, float_type)
  GET_FEATURE_SELF2_M(bid_quantity, int32_t)
  GET_FEATURE_SELF2_M(offer_quantity, int32_t)
  GET_FEATURE_SELF2_M(volume, int32_t)
  GET_FEATURE_SELF2_M(notional, float_type)

  GET_FEATURE_OTHER2_S(inst, product_id)
  GET_FEATURE_OTHER2_S(limit_up, float_type)
  GET_FEATURE_OTHER2_S(limit_down, float_type)

  GET_FEATURE_OTHER2_M(time_micro, int64_t)
  GET_FEATURE_OTHER2_M(bid, float_type)
  GET_FEATURE_OTHER2_M(offer, float_type)
  GET_FEATURE_OTHER2_M(bid_quantity, int32_t)
  GET_FEATURE_OTHER2_M(offer_quantity, int32_t)
  GET_FEATURE_OTHER2_M(volume, int32_t)
  GET_FEATURE_OTHER2_M(notional, float_type)

  //  GET_FEATURE_SELF(inst, product_id)
  //  GET_FEATURE_SELF(time_micro, int64_t)
  //  GET_FEATURE_SELF(bid, float_type)
  //  GET_FEATURE_SELF(offer, float_type)
  //  GET_FEATURE_SELF(bid_quantity, int32_t)
  //  GET_FEATURE_SELF(offer_quantity, int32_t)
  //  GET_FEATURE_SELF(volume, int32_t)
  //  GET_FEATURE_SELF(notional, float_type)
  //  GET_FEATURE_SELF(limit_up, float_type)
  //  GET_FEATURE_SELF(limit_down, float_type)
};

template <class sub_class>
struct fm_feature {
  fm_feature() {}
  explicit fm_feature(const std::string& feautre_name) { init(feautre_name); }
  explicit fm_feature(const uint64_t& fid) { get_feature_id() = fid; }
  inline void init(const std::string& feautre_name) {
    uint64_t& feature_id = get_feature_id();
    feature_id = eal_hash64_fnv1a(feautre_name.c_str(), feautre_name.size());
  }

  std::string get_feature_name() const {
    const uint64_t& fid = static_cast<const sub_class*>(this)->m_feature_id;
    char name[32];
    eal_shm_hash_name(fid, name);
    return name;
  }
  uint64_t& get_feature_id() {
    return static_cast<sub_class*>(this)->m_feature_id;
  }
  const uint64_t& get_feature_id() const {
    return static_cast<const sub_class*>(this)->m_feature_id;
  }
};

template <class sub_class>
struct fm_self_feature /*: public fm_feature<sub_class>*/ {
  // typedefs
  typedef fm_feature<sub_class> base_type;
  // using base_type::get_feature_id;
  // using base_type::get_feature_name;

  fm_self_feature() {}
  fm_self_feature(const std::string& fname, const product_id& oid)
  /*:base_type(fname)*/ {
    (void)fname;
    init(oid);
  }
  fm_self_feature(const std::string& fname, const std::string& oid)
  /* : base_type(fname) */ {
    (void)fname;
    prod_string(&get_trading_instrument(), oid.c_str());
  }
  product_id& get_trading_instrument() {
    return static_cast<sub_class*>(this)->m_trading_instrument;
  }
  const product_id& get_trading_instrument() const {
    return static_cast<const sub_class*>(this)->m_trading_instrument;
  }
  void set_trading_instrument(const product_id& tid) {
    static_cast<sub_class*>(this)->m_trading_instrument = tid;
  }

  inline void init(const std::string& fname, const product_id& oid) {
    (void)fname;
    //    base_type::init(fname);
    set_trading_instrument(oid);
  }
};

template <class sub_class>
struct fm_other_feature : public fm_self_feature<sub_class> {
  // Typedefs
  typedef fm_self_feature<sub_class> self_type;
  typedef const product_id cinstid_t;
  typedef product_id instid_t;

  fm_other_feature() {}

  fm_other_feature(const std::string& feature_name,
                   const std::string& trading_instrument,
                   const std::string& reference_instrument)
      : self_type(feature_name, trading_instrument) {
    prod_string(&get_reference_instrument(), reference_instrument.c_str());
  }

  inline void init(const uint64_t& fid, const product_id& tid,
                   const product_id& rid) {
    this->get_feature_id() = fid;
    this->get_trading_instrument() = tid;
    this->get_reference_instrument() = rid;
  }

  cinstid_t& get_reference_instrument() const {
    return static_cast<const sub_class*>(this)->m_reference_instrument;
  }
  instid_t& get_reference_instrument() {
    return static_cast<sub_class*>(this)->m_reference_instrument;
  }
  void set_reference_instrument(cinstid_t& rid) {
    static_cast<sub_class*>(this)->m_reference_instrument = rid;
  }
};

template <class sub_class, class sub_type>
struct fm_discrete_other_translation_feature
    : public fm_other_feature<sub_class> {
  typedef typename fm_type_traits<sub_type>::float_type float_type;
  typedef china_l1msg<float_type> china_l1msg;
  typedef fast_ema<float_type> ema_type;
  typedef fm_other_feature<sub_class> other_type;
  typedef fm_feature_data<sub_type> data_type;

 protected:
  const data_type* pdata() const {
    const sub_class* sub = static_cast<const sub_class*>(this);
    return static_cast<const data_type*>(sub);
  }
  data_type* pdata() {
    sub_class* sub = static_cast<sub_class*>(this);
    return static_cast<data_type*>(sub);
  }
  const ema_type& m_ema() const { return pdata()->m_ema; }
  ema_type& m_ema() { return pdata()->m_ema; }
  const bool& m_use_diff() const { return pdata()->m_use_diff; }
  bool& m_use_diff() { return pdata()->m_use_diff; }
  const bool& m_corr_negative() const { return pdata()->m_corr_negative; }
  bool& m_corr_negative() { return pdata()->m_corr_negative; }

 public:
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

  inline void init(float_type ratio_mics, bool use_diff, bool corr_negative) {
    m_ema().init(1.0 / ratio_mics);
    m_use_diff() = use_diff;
    m_corr_negative() = corr_negative;
  }

  template <class U = float_type>
  float_type calculatedRatio(
      float_type mid, float_type other_mid,
      typename std::enable_if<std::is_same<U, double>::value, U>::type* =
          0) const {
    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    __m256d value;
    value = _mm256_set_pd((mid + other_mid), (mid * other_mid),
                          (other_mid - mid), (other_mid / mid));
    float_type* dv = (float_type*)&value;
    return dv[i];

    //    value[3] = (mid + other_mid);
    //    value[2] = (mid * other_mid);
    //    value[1] = (other_mid - mid);
    //    value[0] = (other_mid / mid);
    //    return value[i];
    //    return m_corr_negative()
    //               ? (m_use_diff() ? (mid + other_mid) : (mid * other_mid))
    //               : (m_use_diff() ? (other_mid - mid) : (other_mid / mid));
  }

  template <class U = float_type>
  float_type calculatedRatio(
      float_type mid, float_type other_mid,
      typename std::enable_if<std::is_same<U, float>::value, U>::type* =
          0) const {
    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    __m128 value;
    value = _mm_set_ps((mid + other_mid), (mid * other_mid), (other_mid - mid),
                       (other_mid / mid));
    float_type* dv = (float_type*)&value;
    return dv[i];

    //    value[3] = (mid + other_mid);
    //    value[2] = (mid * other_mid);
    //    value[1] = (other_mid - mid);
    //    value[0] = (other_mid / mid);
    //    return value[i];
    //    return m_corr_negative()
    //               ? (m_use_diff() ? (mid + other_mid) : (mid * other_mid))
    //               : (m_use_diff() ? (other_mid - mid) : (other_mid / mid));
  }
  template <class U = float_type>
  float_type translatedPrice(
      float_type other_price,
      typename std::enable_if<std::is_same<U, float>::value, U>::type* =
          0) const {
    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    __m128 value;
    value = _mm_set_ps((m_ema().get_value() - other_price),
                       (m_ema().get_value() / other_price),
                       (other_price - m_ema().get_value()),
                       (other_price / m_ema().get_value()));
    float_type* dv = (float_type*)&value;
    return dv[i];
    //    return m_corr_negative()
    //               ? (m_use_diff() ? (m_ema().get_value() - other_price)
    //                               : (m_ema().get_value() / other_price))
    //               : (m_use_diff() ? (other_price - m_ema().get_value())
    //                               : (other_price / m_ema().get_value()));
  }
  template <class U = float_type>
  float_type translatedPrice(
      float_type other_price,
      typename std::enable_if<std::is_same<U, double>::value, U>::type* =
          0) const {
    //    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    //    float_type a = (m_ema().get_value() - other_price);
    //    float_type b = (m_ema().get_value() / other_price);
    //    float_type vec[4] = {-b, -a, b, a};
    //    return vec[i];
    //    __m256 value;
    //    value = _mm256_set_pd((m_ema().get_value() - other_price),
    //                          (m_ema().get_value() / other_price),
    //                          (other_price - m_ema().get_value()),
    //                          (other_price / m_ema().get_value()));
    //    float_type* dv = (float_type*)&value;
    //    return dv[i];
    return m_corr_negative()
               ? (m_use_diff() ? (m_ema().get_value() - other_price)
                               : (m_ema().get_value() / other_price))
               : (m_use_diff() ? (other_price - m_ema().get_value())
                               : (other_price / m_ema().get_value()));
  }

  template <class U = float_type>
  float_type translatedDelta(
      float_type other_delta, float_type other_mid,
      typename std::enable_if<std::is_same<U, double>::value, U>::type* =
          0) const {
    //    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    //    __m256 value;
    //    // 3  other_delta  2  other_delta * m_ema().get_value() /
    //    //                                          other_mid / other_mid
    //    // other_delta 0 other_delta / m_ema().get_value()
    //    value = _mm256_set_pd(
    //        other_delta, other_delta * m_ema().get_value() / other_mid /
    //        other_mid, other_delta, other_delta / m_ema().get_value());
    //    float_type* dv = (float_type*)&value;
    //    return dv[i];
    return m_use_diff()
               ? other_delta
               : (m_corr_negative() ? other_delta * m_ema().get_value() /
                                          other_mid / other_mid
                                    : other_delta / m_ema().get_value());
  }
  template <class U>
  float_type translatedDelta(
      float_type other_delta, float_type other_mid,
      typename std::enable_if<std::is_same<U, float>::value, U>::type* =
          nullptr) const {
    int i = ((m_corr_negative()) << 1) + (m_use_diff());
    __m128 value;
    // 3  other_delta  2  other_delta * m_ema().get_value() /
    //                                          other_mid / other_mid
    // other_delta 0 other_delta / m_ema().get_value()
    value = _mm_set_ps(
        other_delta, other_delta * m_ema().get_value() / other_mid / other_mid,
        other_delta, other_delta / m_ema().get_value());
    float_type* dv = (float_type*)&value;
    return dv[i];
    //    return m_use_diff()
    //               ? other_delta
    //               : (m_corr_negative() ? other_delta * m_ema().get_value() /
    //                                          other_mid / other_mid
    //                                    : other_delta / m_ema().get_value());
  }

  float_type translatedBid(float_type other_bid, float_type other_offer) const {
    return m_corr_negative() ? translatedPrice<float_type>(other_offer)
                             : translatedPrice<float_type>(other_bid);
  }

  float_type translatedOffer(float_type other_bid,
                             float_type other_offer) const {
    return m_corr_negative() ? translatedPrice<float_type>(other_bid)
                             : translatedPrice<float_type>(other_offer);
  }

  float_type signedSignal(float_type signal) const {
    return m_corr_negative() ? (-signal) : signal;
  }
};

}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_FEATUREBASE_H_
