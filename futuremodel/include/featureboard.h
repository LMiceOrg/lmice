/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_FEATUREBOARD_H_
#define FUTUREMODEL_INCLUDE_FEATUREBOARD_H_

#include <stdint.h>

#include <array>

#include "eal/lmice_eal_common.h"
#include "eal/lmice_eal_rwlock.h"

#include "include/chinal1msg.h"
#include "lib/FemasAPIv2/linux64/include/USTPFtdcUserApiStruct.h"
namespace lmice {

struct feature_worker {
  int32_t m_worker_status; /* start, working stop */
  lmice_rwlock_t m_worker_lock;
  uint32_t m_worker_sigbegin;
  uint32_t m_worker_sigend;
} forcepack(8);

template <class float_type, int feature_size, int reference_size, int pack_size>
struct feature_group {
  typedef std::array<float_type, feature_size> af_type;
  typedef std::array<product_id, reference_size> ar_type;
  enum {
    m_aligned_size = ((feature_size + pack_size - 1) / pack_size) * pack_size,
    m_feature_size = feature_size,
    m_pack_size = pack_size,
    m_referemce_size = reference_size
  };
  af_type forcepack(pack_size) m_signals;
  af_type forcepack(pack_size) m_weights;
  af_type forcepack(pack_size) m_weighted_signals;
  ar_type forcepack(pack_size) m_reference_instruments;

  // Open forecast
  float_type m_open_forecast;  // (1+\sigma(signal*weight))*(ask+bid)/2
  // Market amplifier
  float_type m_market_amp;

  product_id m_trading_instruments;

  // Order insert (Femas2)
  CUstpFtdcInputOrderField m_order;

  // Order price (refto) m_order.LimitPrice
  double& m_order_price;

  // Order direction (refto) m_order.Direction
  char& m_order_direction;

  // Risk order size (refto) m_order.Volume
  int& m_order_size;

} forcepack(pack_size);

template <class float_type, int worker_size, int feature_size, int quote_size,
          int quote_depth, int group_size>
struct feature_board {
  uint32_t m_worker_count; /* total running workers */
  uint32_t m_signal_count; /* total used signals */
  uint32_t m_msg_count;    /* total used quotes */
  int32_t m_msg_id;        /* current fired msg */

  volatile int64_t m_worker_flag; /* quit flag */

  volatile int64_t m_worker_result; /* count of finished worker */

  // worker[0] : Openforecast
  // worker[1...]: market/risk/portfolio/Close
  std::array<feature_worker, worker_size + 1> m_workers;
  // feature_worker m_workers[LM_MAX_WORKERSIZE];
  std::array<feature_group<float_type, feature_size, 8, 16>, group_size>
      m_feature_groups;
  // float m_signals[LM_MAX_SIGNALSIZE];

  // ChinaL1Msg m_cur_msgs[LM_MAX_L1MSGSIZE * 2];
  // ChinaL1Msg m_pre_msgs[LM_MAX_L1MSGSIZE];
  std::array<china_l1msg<float_type>, quote_size * quote_depth> m_quotes;
};

struct close {};
struct market {

};

struct risk {

};

struct portfolio {

};

}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_FEATUREBOARD_H_
