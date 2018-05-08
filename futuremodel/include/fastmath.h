/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_FASTMATH_H_
#define FUTUREMODEL_INCLUDE_FASTMATH_H_

#include <immintrin.h>
#include <stdint.h>

#include "eal/lmice_eal_common.h"

forceinline double noexport fast_exp(double x) {
  double y = 1.0 + x / 256.0;
  for (int i = 0; i < 8; ++i) y *= y;
  return y;
}

forceinline float noexport fast_exp(float x) {
  float y = 1.0f + x / 256.0f;
  for (int i = 0; i < 8; ++i) y *= y;
  return y;
}

///* max. rel. error <= 1.73e-3 on [-87,88] */
// forceinline double fast_exp(double x) {
//  return (120 + x * (120 + x * (60 + x * (20 + x * (5 + x))))) *
//  0.0083333333f;
//}

forceinline double noexport fast_abs(double x) {
  union uc {
    double f64;
    uint64_t i64;
  };
  uc u;
  u.f64 = x;
  u.i64 &= ((1ULL << 63) - 1);
  return u.f64;
}

forceinline float noexport fast_abs(float x) {
  union uc {
    float f32;
    uint32_t i32;
  };
  uc u;
  u.f32 = x;
  u.i32 &= ((1U << 31) - 1);
  return u.f32;
}

forceinline double noexport fast_sqrt(double n) {
  return _mm_cvtsd_f64(_mm_sqrt_pd(_mm_set_sd(n)));
}

forceinline float noexport fast_sqrt(float n) {
  return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(n)));
  //  double result = 0;
  //  __asm__(
  //      "fldl %1\n\t"  // st(0)=>st(1), st(0)=lgth . FLDL means load double
  //      float "fsqrt\n\t"    // st(0) = square root st(0) : "=&t"(result) :
  //      "m"(n));
  //  return result;
}

namespace lmice {

template <class float_type>
class fm_ema {
 public:
  fm_ema() {}
  explicit fm_ema(float_type decay)
      : m_initialized(false), m_decay(decay), m_value(0), m_last_event(0) {}
  inline void update(float_type value, float_type event) {
    if (!m_initialized) {
      m_value = value;
      m_initialized = true;
    } else {
      float_type delta = event - m_last_event;
      float_type u = fast_exp(-m_decay * delta);
      m_value = m_value * u + value * (1 - u);
    }

    m_last_event = event;
  }
  inline void init(float_type decay) {
    m_decay = decay;
    m_initialized = false;
    m_value = 0;
    m_last_event = 0;
  }
  inline void reset() {
    m_initialized = false;
    m_value = 0;
    m_last_event = 0;
  }
  inline float_type get_value() const { return m_value; }

 private:
  bool m_initialized;
  float_type m_decay;
  float_type m_value;
  float_type m_last_event;
};
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_FASTMATH_H_
