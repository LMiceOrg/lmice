/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_FASTMATH_H_
#define FUTUREMODEL_INCLUDE_FASTMATH_H_

#include <immintrin.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "eal/lmice_eal_atomic.h"
#include "eal/lmice_eal_common.h"
#define FAST_PS_CONST(Name, Val) \
  static const float fast_ps_##Name[4] forcepack(16) = {Val, Val, Val, Val}
#define FAST_PI32_CONST(Name, Val) \
  static const int fast_pi32_##Name[4] forcepack(16) = {Val, Val, Val, Val}

FAST_PI32_CONST(0x7f, 0x7f);

FAST_PS_CONST(1, 1.0f);
FAST_PS_CONST(0p5, 0.5f);

FAST_PS_CONST(exp_hi, 88.3762626647949f);
FAST_PS_CONST(exp_lo, -88.3762626647949f);

FAST_PS_CONST(cephes_LOG2EF, 1.44269504088896341);
FAST_PS_CONST(cephes_exp_C1, 0.693359375);
FAST_PS_CONST(cephes_exp_C2, -2.12194440e-4);

FAST_PS_CONST(cephes_exp_p0, 1.9875691500E-4);
FAST_PS_CONST(cephes_exp_p1, 1.3981999507E-3);
FAST_PS_CONST(cephes_exp_p2, 8.3334519073E-3);
FAST_PS_CONST(cephes_exp_p3, 4.1665795894E-2);
FAST_PS_CONST(cephes_exp_p4, 1.6666665459E-1);
FAST_PS_CONST(cephes_exp_p5, 5.0000001201E-1);

#define fast_v4sf __m128d
#define fast_v4si __m128i

forceinline fast_v4sf noexport fast_exp_ps(fast_v4sf x) {
  fast_v4sf tmp = _mm_setzero_ps(), fx;
  fast_v4si emm0;

  fast_v4sf one = *(fast_v4sf*)fast_ps_1;

  x = _mm_min_ps(x, *(fast_v4sf*)fast_ps_exp_hi);
  x = _mm_max_ps(x, *(fast_v4sf*)fast_ps_exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
  fx = _mm_mul_ps(x, *(fast_v4sf*)fast_ps_cephes_LOG2EF);
  fx = _mm_add_ps(fx, *(fast_v4sf*)fast_ps_0p5);

  /* how to perform a floorf with SSE: just below */
  emm0 = _mm_cvttps_epi32(fx);
  tmp = _mm_cvtepi32_ps(emm0);

  /* if greater, substract 1 */
  fast_v4sf mask = _mm_cmpgt_ps(tmp, fx);
  mask = _mm_and_ps(mask, one);
  fx = _mm_sub_ps(tmp, mask);

  tmp = _mm_mul_ps(fx, *(fast_v4sf*)fast_ps_cephes_exp_C1);
  fast_v4sf z = _mm_mul_ps(fx, *(fast_v4sf*)fast_ps_cephes_exp_C2);
  x = _mm_sub_ps(x, tmp);
  x = _mm_sub_ps(x, z);

  z = _mm_mul_ps(x, x);

  fast_v4sf y = *(fast_v4sf*)fast_ps_cephes_exp_p0;
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(fast_v4sf*)fast_ps_cephes_exp_p1);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(fast_v4sf*)fast_ps_cephes_exp_p2);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(fast_v4sf*)fast_ps_cephes_exp_p3);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(fast_v4sf*)fast_ps_cephes_exp_p4);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(fast_v4sf*)fast_ps_cephes_exp_p5);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, x);
  y = _mm_add_ps(y, one);

  /* build 2^n */
  emm0 = _mm_cvttps_epi32(fx);
  emm0 = _mm_add_epi32(emm0, *(fast_v4si*)fast_pi32_0x7f);
  emm0 = _mm_slli_epi32(emm0, 23);
  fast_v4sf pow2n = _mm_castsi128_ps(emm0);

  y = _mm_mul_ps(y, pow2n);
  return y;
}

forceinline float noexport fast_exp_ss(float x) {
  fast_v4sf px = _mm_load1_ps(&x);
  fast_v4sf py = fast_exp_ps(px);
  return py[0];
}

forceinline void fast_datetime(char* dt, int size) {
  struct tm tmnow;
  time_t tnow;
  time(&tnow);
  localtime_r(&tnow, &tmnow);
  snprintf(dt, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tmnow.tm_year + 1900,
           tmnow.tm_mon + 1, tmnow.tm_mday, tmnow.tm_hour, tmnow.tm_min,
           tmnow.tm_sec, 0);
}

forceinline int64_t noexport fast_totime(const char* stime) {
  int64_t dhour = ((stime[0] & 0xf) * 10 + (stime[1] & 0xf)) * 3600000000LL;
  int64_t dmin = ((stime[3] & 0xf) * 10 + (stime[4] & 0xf)) * 60000000LL;
  int64_t dsec = ((stime[6] & 0xf) * 10 + (stime[7] & 0xf)) * 1000000LL;
  int64_t dmsc = (stime[9] & 0xf) * 100000LL + (stime[10] & 0xf) * 10000LL +
                 (stime[11] & 0xf) * 1000LL;
  return dhour + dmin + dsec + dmsc;
}

forceinline int64_t noexport fast_totime(const char* sc_date,
                                         const char* sc_time,
                                         const char* bs_date, int64_t bs_time) {
  int64_t result;
  // convert to time
  result = fast_totime(sc_time) + bs_time;
  // adjust date
  int64_t value;

  //  // full version
  //  value =
  //      /*year*/
  //      ((sdate[0] - base_date[0]) * 1000 + (sdate[1] - base_date[1]) * 100 +
  //       (sdate[2] - base_date[2]) * 100 + (sdate[3] - base_date[3]) * 1) *
  //          365 * 86400 * 1000000LL
  //      /* month */
  //      + ((sdate[4] - base_date[4]) * 10 + (sdate[5] - base_date[5]) * 1) *
  //      30 *
  //            86400 * 1000000LL
  //      /* day */
  //      + ((sdate[6] - base_date[6]) * 10 + (sdate[7] - base_date[7]) * 1) *
  //            86400 * 1000000LL;

  // only see month day
  // Little-endian convert
  int64_t cur_date = __builtin_bswap64(*(const int64_t*)sc_date);
  int64_t base_date = __builtin_bswap64(*(const int64_t*)bs_date);

  int base_month =
      ((base_date & 0xff000000) >> 24) * 10 + ((base_date & 0xff0000) >> 16);
  int base_day = ((base_date & 0xff00) >> 8) * 10 + ((base_date & 0xff) >> 0);
  int cur_month =
      ((cur_date & 0xff000000) >> 24) * 10 + ((cur_date & 0xff0000) >> 16);
  int cur_day = ((cur_date & 0xff00) >> 8) * 10 + ((cur_date & 0xff) >> 0);

  int month_diff = cur_month - base_month;
  int day_diff = cur_day - base_day;

  value = (month_diff * 30 + day_diff) * 86400LL * 1000000LL;

  result += value;
  return result;
}

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
}

#include <cmath>
namespace lmice {

template <class float_type>
class fast_ema {
 public:
  fast_ema() : m_decay(0), m_value(0), m_last_event(0) {}

  inline void update(float_type value, float_type event) {
    float_type delta = -m_decay * (event - m_last_event);
    float_type u = fast_exp(delta);  // fast_exp(delta);
    m_value = m_value * u + value * (1 - u);

    m_last_event = event;
  }
  inline void init(float_type decay) {
    m_decay = decay;
    m_value = 0;
    m_last_event = 0;
  }
  inline void init(float_type value, float_type event) {
    m_value = value;
    m_last_event = event;
  }
  inline void reset() {
    m_value = 0;
    m_last_event = 0;
  }
  inline float_type get_event() const { return m_last_event; }
  inline float_type get_value() const { return m_value; }

 private:
  float_type m_decay;
  float_type m_value;
  float_type m_last_event;
};
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_FASTMATH_H_
