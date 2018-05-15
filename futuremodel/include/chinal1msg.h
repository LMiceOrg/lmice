/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_CHINAL1MSG_H_
#define FUTUREMODEL_INCLUDE_CHINAL1MSG_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <array>

#include "eal/lmice_eal_common.h"

#if !defined(FEATURE_PRODUCT_ID_TYPE)
#define FEATURE_PRODUCT_ID_TYPE

#ifdef __cplusplus
namespace lmice {

#define QUOTE_SIZE(n) (n)
#define QUOTE_DEPTH(n) (n)
#define QUOTE_POS(n) (n)
#define QUOTE_SELF_POS(n) (n)
#define QUOTE_OTHER_POS(n) (n)

typedef int quote_descriptor;
typedef quote_descriptor quote_d;

union product_id {
  int64_t i64;
  char s8[8];
  product_id() : i64(0) {}
  product_id(int64_t i) : i64(i) {}
  product_id(const char* s) {
    //    if (strlen(s) >= 8) {
    //      throw "product_id ctor: too long";
    //    }
    i64 = 0;
    snprintf(s8, sizeof(s8), "%s", s);
  }
  operator const char*() const { return s8; }
  operator int64_t() const { return i64; }
  void operator=(const product_id& r2) { i64 = r2.i64; }
} forcepack(8);

forceinline void prod_copy(product_id* nid, const product_id& oid) {
  nid->i64 = oid.i64;
}

forceinline void prod_string(product_id* nid, const char* oid) {
  size_t size = strlen(oid);
  if (size < sizeof(product_id)) {
    nid->i64 = 0;
    memcpy(nid->s8, oid, size);
  } else {
    const int64_t* o = reinterpret_cast<const int64_t*>(oid);
    nid->i64 = *o;
  }
}

forceinline bool prod_equal(const product_id& nid, const product_id& oid) {
  return nid.i64 == oid.i64;
}

forceinline bool operator==(const product_id& r1, const product_id& r2) {
  return r1.i64 == r2.i64;
}
forceinline bool operator<(const product_id& r1, const product_id& r2) {
  return r1.i64 < r2.i64;
}

template <class T, class float_type>
struct china_l1msg_handler {
  inline const product_id& get_inst() const {
    const T* t = static_cast<const T*>(this);
    return t->m_inst;
  }
  inline int64_t get_time() const {
    const T* t = static_cast<const T*>(this);
    return t->m_time_micro;
  }
  inline float_type get_bid() const {
    const T* t = static_cast<const T*>(this);
    return t->m_bid;
  }
  inline float_type get_offer() const {
    const T* t = static_cast<const T*>(this);
    return t->m_offer;
  }
  inline float_type get_bid_quantity() const {
    const T* t = static_cast<const T*>(this);
    return t->m_bid_quantity;
  }
  inline float_type get_offer_quantity() const {
    const T* t = static_cast<const T*>(this);
    return t->m_offer_quantity;
  }
  inline float_type get_volume() const {
    const T* t = static_cast<const T*>(this);
    return t->m_volume;
  }
  inline float_type get_notional() const {
    const T* t = static_cast<const T*>(this);
    return t->m_notional;
  }
  inline float_type get_limit_up() const {
    const T* t = static_cast<const T*>(this);
    return t->m_limit_up;
  }
  inline float_type get_limit_down() const {
    const T* t = static_cast<const T*>(this);
    return t->m_limit_down;
  }
};

template <class float_type>
struct china_l1msg_base {
  //  product_id m_inst;
  int64_t m_time_micro;  // time in epoch micro seconds
  float_type m_bid;
  float_type m_offer;
  int32_t m_bid_quantity;
  int32_t m_offer_quantity;
  int32_t m_volume;
  float_type m_notional;
  //  float_type m_limit_up;
  //  float_type m_limit_down;
} forcepack(64);

template <class float_type>
struct china_l1msg
    : public china_l1msg_base<float_type>,
      public china_l1msg_handler<china_l1msg<float_type>, float_type> {};

template <class sub_class>
struct fm_type_traits;

template <template <typename> class sub_class, typename ft_type>
struct fm_type_traits<sub_class<ft_type>> {
  typedef ft_type float_type;
};

template <template <typename, int, int> class sub_class, typename ft_type,
          int quote_size, int quote_depth>
struct fm_type_traits<sub_class<ft_type, quote_size, quote_depth>> {
  typedef ft_type float_type;
  enum { m_quote_size = quote_size, m_quote_depth = quote_depth };
};

template <template <typename, int, int, int> class sub_class, typename ft_type,
          int quote_size, int quote_depth, int contract_size>
struct fm_type_traits<
    sub_class<ft_type, quote_size, quote_depth, contract_size>> {
  typedef ft_type float_type;
  enum {
    m_quote_size = quote_size,
    m_quote_depth = quote_depth,
    m_contract_size = contract_size
  };
};

template <template <typename, int, int, int, int> class sub_class,
          typename ft_type, int quote_size, int quote_depth, int contract_size,
          int feature_size>
struct fm_type_traits<
    sub_class<ft_type, quote_size, quote_depth, contract_size, feature_size>> {
  typedef ft_type float_type;
  typedef china_l1msg<float_type> china_l1msg_type;
  enum {
    m_quote_size = quote_size,
    m_quote_depth = quote_depth,
    m_contract_size = contract_size,
    m_feature_size = feature_size
  };
};

// template <template <typename, int, int> class sub_class, typename ft_type,
//          int quote_size, int quote_depth>
// struct fm_type_traits<sub_class<ft_type, quote_size, quote_depth>> {
//  typedef ft_type float_type;
//};

#define USING_FEATURE_METHOD_SELF(name) \
  using msg_type::get_##name;           \
  using msg_type::get_cur_##name;       \
  using msg_type::get_prev_##name;

#define USING_FEATURE_METHOD_OTHER(name) \
  using msg_type::get_other_##name;      \
  using msg_type::get_cur_other_##name;  \
  using msg_type::get_prev_other_##name;

#define USING_FEATURE_METHODS()              \
  USING_FEATURE_METHOD_SELF(inst)            \
  USING_FEATURE_METHOD_SELF(time_micro)      \
  USING_FEATURE_METHOD_SELF(bid)             \
  USING_FEATURE_METHOD_SELF(offer)           \
  USING_FEATURE_METHOD_SELF(bid_quantity)    \
  USING_FEATURE_METHOD_SELF(offer_quantity)  \
  USING_FEATURE_METHOD_SELF(volume)          \
  USING_FEATURE_METHOD_SELF(notional)        \
  USING_FEATURE_METHOD_SELF(limit_up)        \
  USING_FEATURE_METHOD_SELF(limit_down)      \
  USING_FEATURE_METHOD_OTHER(inst)           \
  USING_FEATURE_METHOD_OTHER(time_micro)     \
  USING_FEATURE_METHOD_OTHER(bid)            \
  USING_FEATURE_METHOD_OTHER(offer)          \
  USING_FEATURE_METHOD_OTHER(bid_quantity)   \
  USING_FEATURE_METHOD_OTHER(offer_quantity) \
  USING_FEATURE_METHOD_OTHER(volume)         \
  USING_FEATURE_METHOD_OTHER(notional)       \
  USING_FEATURE_METHOD_OTHER(limit_up)       \
  USING_FEATURE_METHOD_OTHER(limit_down)     \
  using msg_type::set_self_quote;            \
  using msg_type::set_other_quote;           \
  using msg_type::get_signal;                \
  using msg_type::set_signal;

//#define GET_SELF(name, type)                                                 \
//  template <int depth_pos = 0>                                               \
//  inline const type& get_##name() const {                                    \
//    const sub_class* t = static_cast<const sub_class*>(this);                \
//    static_assert(quote_pos < T::m_quote_size, "quote must < quote_size");   \
//    static_assert(depth_pos < T::m_quote_depth, "depth must < quote_count"); \
//    return t->m_quotes.m_##name[quote_pos * T::m_quote_depth + depth_pos];   \
//  }                                                                          \
//  inline const type& get_cur_##name() const { return get_##name<0>(); }      \
//  inline const type& get_prev_##name() const { return get_##name<1>(); }     \
//  template <int depth_pos = 0>                                               \
//  inline void set_##name(const type& name) {                                 \
//    sub_class* t = static_cast<sub_class*>(this);                            \
//    static_assert(quote_pos < T::m_quote_size, "quote must < quote_size");   \
//    static_assert(depth_pos < T::m_quote_depth, "depth must < quote_count"); \
//    t->m_##name[quote_pos * T::m_quote_depth + depth_pos] = name;            \
//  }

// template <class sub_class, int quote_pos>
// struct fm_chinal1_msg_handler {
//  typedef typename fm_type_traits<sub_class>::float_type float_type;
//  typedef sub_class T;
//  GET_SELF(inst, product_id)
//  GET_SELF(time_micro, int64_t)
//  GET_SELF(bid, float_type)
//  GET_SELF(offer, float_type)
//  GET_SELF(bid_quantity, int32_t)
//  GET_SELF(offer_quantity, int32_t)
//  GET_SELF(volume, int32_t)
//  GET_SELF(notional, float_type)
//  GET_SELF(limit_up, float_type)
//  GET_SELF(limit_down, float_type)
//};

//#define GET_OTHER(name, type)                                                \
//  template <int depth_pos = 0>                                               \
//  inline const type& get_other_##name() const {                              \
//    const sub_class* t = static_cast<const sub_class*>(this);                \
//    static_assert(quote_pos < T::m_quote_size, "quote must < quote_size");   \
//    static_assert(depth_pos < T::m_quote_depth, "depth must < quote_count"); \
//    return t->m_quotes.m_##name[quote_pos * T::m_quote_depth + depth_pos];   \
//  }                                                                          \
//  inline const type& get_cur_other_##name() const {                          \
//    return get_other_##name<0>();                                            \
//  }                                                                          \
//  inline const type& get_prev_other_##name() const {                         \
//    return get_other_##name<1>();                                            \
//  }

// template <class sub_class, int quote_pos>
// struct fm_chinal1_msg_other_handler {
//  typedef typename fm_type_traits<sub_class>::float_type float_type;
//  typedef sub_class T;

//  GET_OTHER(inst, product_id)
//  GET_OTHER(time_micro, int64_t)
//  GET_OTHER(bid, float_type)
//  GET_OTHER(offer, float_type)
//  GET_OTHER(bid_quantity, int32_t)
//  GET_OTHER(offer_quantity, int32_t)
//  GET_OTHER(volume, int32_t)
//  GET_OTHER(notional, float_type)
//  GET_OTHER(limit_up, float_type)
//  GET_OTHER(limit_down, float_type)
//};

template <class sub_class>
struct fm_chinal1_msg_handler {
  int get_quote_fd(const char* oid) const {
    int i = 0;
    const sub_class* sub = static_cast<const sub_class*>(this);
    product_id tid;
    prod_string(&tid, oid);
    for (i = 0; i < sub->m_quote_size; ++i) {
      if (sub->m_aliases[i] == tid) break;
    }
    return i;
  }
};

template <class sub_class>
struct fm_chinal1_msg_data {
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_quote_count = m_quote_size * m_quote_depth
  };
  typedef typename fm_type_traits<sub_class>::float_type float_type;
  typedef std::array<china_l1msg<float_type>, m_quote_depth> quote_msg_type;
  static_assert((m_quote_count % 4) == 0,
                "fm_chinal1_msg quote_size x quote_depth must 4x");

  std::array<quote_msg_type, m_quote_count> m_msgs forcepack(64);
  // std::array<china_l1msg<float_type>, m_quote_count> m_msgs;

  //  std::array<int64_t, m_quote_count> m_time_micro;  // epoch micro seconds
  //  std::array<float_type, m_quote_count> m_bid;
  //  std::array<float_type, m_quote_count> m_offer;
  //  std::array<int32_t, m_quote_count> m_bid_quantity;
  //  std::array<int32_t, m_quote_count> m_offer_quantity;
  //  std::array<int32_t, m_quote_count> m_volume;
  //  std::array<float_type, m_quote_count> m_notional;
  // product_id
  std::array<product_id, m_quote_size> m_inst;
  // alias
  std::array<product_id, m_quote_size> m_aliases;
  //  limit_up
  std::array<float_type, m_quote_size> m_limit_up;
  //  limit_down
  std::array<float_type, m_quote_size> m_limit_down;

} forcepack(128);

template <class float_type, int quote_size, int quote_depth>
struct fm_chinal1_msg
    : public fm_chinal1_msg_data<
          fm_chinal1_msg<float_type, quote_size, quote_depth>>,
      public fm_chinal1_msg_handler<
          fm_chinal1_msg<float_type, quote_size, quote_depth>> {};

}  // namespace lmice

#else
typedef char product_id[8];
forceinline void prod_copy(product_id nid, const product_id oid) {
  if (nid == oid) return;
  int64_t* n = reinterpret_cast<int64_t*>(nid);
  const int64_t* o = reinterpret_cast<const int64_t*>(oid);
  *n = *o;
}
forceinline void prod_string(product_id nid, const char* oid) {
  int64_t* n = reinterpret_cast<int64_t*>(nid);
  const int64_t* o = reinterpret_cast<const int64_t*>(oid);
  size_t size = strlen(oid);
  if (size < sizeof(product_id)) {
    *n = 0;
    memcpy(nid, oid, size);
  } else {
    *n = *o;
  }
}

forceinline int prod_equal(const product_id nid, const product_id oid) {
  const int64_t* n = reinterpret_cast<const int64_t*>(nid);
  const int64_t* o = reinterpret_cast<const int64_t*>(oid);
  if (*n == *o)
    return 0;
  else
    return 1;
}
#endif

// #ifdef __cplusplus

// namespace lmice {
// struct product_id {
//  ::product_id data;
//  operator const ::product_id&() const { return data; }
//  operator ::product_id&() { return data; }
//  operator int64_t() const { return *reinterpret_cast<const int64_t*>(this); }
//  product_id(const product_id& oid) { ::prod_copy(this->data, oid.data); }
//  product_id(const std::string& oid) {
//    size_t size = oid.size();
//    if (size < sizeof(product_id)) {
//      memset(this->data, 0, sizeof(this->data));
//      memcpy(this->data, oid.c_str(), size);
//    } else {
//      const int64_t* o = reinterpret_cast<const int64_t*>(oid.c_str());
//      int64_t* n = reinterpret_cast<int64_t*>(this->data);
//      *n = *o;
//    }
//  }
//};
// forceinline noexport void prod_copy(product_id& nid, const product_id& oid) {
//  if (&nid == &oid) return;
//  int64_t* n = reinterpret_cast<int64_t*>(nid.data);
//  const int64_t* o = reinterpret_cast<const int64_t*>(oid.data);
//  *n = *o;
//}

// bool operator==(const product_id& nid, const product_id& oid) {
//  int64_t n = nid;
//  int64_t o = oid;
//  return n == o;
// }
// }  // namespace lmice
// #endif
#endif

#endif  // FUTUREMODEL_INCLUDE_CHINAL1MSG_H_
