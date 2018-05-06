#ifndef FASTMATH_H
#define FASTMATH_H

#include "eal/lmice_eal_common.h"

#include <immintrin.h>
#include <stdint.h>

forceinline double noexport fast_exp(double x) {
  double y = 1.0 + x / 256.0;
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

#endif  // FASTMATH_H
