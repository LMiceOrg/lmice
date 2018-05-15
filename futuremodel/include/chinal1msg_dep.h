#ifndef CHINAL1MSG_DEP_H
#define CHINAL1MSG_DEP_H
#include "eal/lmice_eal_common.h"

#include <stdint.h>

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

template <class T, class float_type>
struct L1MsgHandler {
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

template <class tp>
struct ChinaL1MsgT {
  typedef tp float_type;

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

template <class tp>
struct ChinaL1MsgBase : public L1MsgHandler<ChinaL1MsgBase<tp>, tp>,
                        public ChinaL1MsgT<tp> {
  typedef tp float_type;
};

typedef ChinaL1MsgBase<double> ChinaL1Msg;
typedef ChinaL1MsgBase<float> ChinaL1MsgFast;

#endif  // CHINAL1MSG_DEP_H
