/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_CHINAL1MSG_H_
#define FUTUREMODEL_INCLUDE_CHINAL1MSG_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eal/lmice_eal_common.h"

#if !defined(FEATURE_PRODUCT_ID_TYPE)
#define FEATURE_PRODUCT_ID_TYPE

#ifdef __cplusplus
namespace lmice {

union product_id {
  int64_t i64;
  char s8[8];
  product_id() : i64(0) {}
  product_id(int64_t i) : i64(i) {}
  product_id(const char* s) {
    if (strlen(s) >= 8) {
      throw "product_id ctor: too long";
    }
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
  product_id m_inst;
  int64_t m_time_micro;  // time in epoch micro seconds
  float_type m_bid;
  float_type m_offer;
  int32_t m_bid_quantity;
  int32_t m_offer_quantity;
  int32_t m_volume;
  float_type m_notional;
  float_type m_limit_up;
  float_type m_limit_down;
} forcepack(8);

template <class float_type>
struct china_l1msg
    : public china_l1msg_base<float_type>,
      public china_l1msg_handler<china_l1msg<float_type>, float_type> {};

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
