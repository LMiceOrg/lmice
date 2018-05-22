/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef FUTUREMODEL_INCLUDE_PORTFOLIO_H_
#define FUTUREMODEL_INCLUDE_PORTFOLIO_H_

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

/** EAL library */
#include "eal/lmice_eal_common.h"

/** IACA library */
#include "iaca/iacaMarks.h"

/** FutureModel library */
#include "include/chinal1msg.h"  // ChinaL1 message
#include "include/chinal1msg_dep.h"
#include "include/contract.h"
#include "include/openfeature.h"
#include "include/quotefeature.h"  // Quote feature
//#include "include/variantfeature.h"

#include "include/quote/udp_recv_dce.h"

#include "include/rollscheme.h"
namespace lmice {

template <class sub_class>
struct fm_portfolio_handler {
 private:
  typedef typename fm_type_traits<sub_class>::float_type float_type;
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_contract_size = fm_type_traits<sub_class>::m_contract_size,
    m_feature_size = fm_type_traits<sub_class>::m_feature_size,
    m_align_size = 4
  };
  typedef china_l1msg<float_type> msg_type;
  typedef typename fm_open_feature_type<float_type, m_quote_size, m_quote_depth,
                                        m_contract_size, m_feature_size>::type
      open_type;
  typedef fm_open_feature<open_type> open_feature_type;
  typedef std::array<msg_type, m_quote_depth> quote_msg_type;
  typedef fm_quote_feature<open_type> quote_feature_type;

 public:
  feature_range& get_next_ref_range(int quote_fd) {
    sub_class* sub = static_cast<sub_class*>(this);

    feature_range* prange = &sub->m_quote_features.m_ref_range[quote_fd][0];
    for (int i = 0; i < sub->m_quote_features.MAX_REFERENCE_COUNT; ++i) {
      if (prange->m_feature_end == 0) return *prange;
    }
    printf("quote_features ref_range overflow\n");
    return *prange;
  }

  int get_quote_fd(const char* oid) const {
    const sub_class* sub = static_cast<const sub_class*>(this);
    return sub->m_quotes.get_quote_fd(oid);
  }

  /*! init base time */
  void init(struct tm tm_now) {
    sub_class* sub = static_cast<sub_class*>(this);
    int64_t now = 0;

    /*! init base time */
    tm_now.tm_hour = 0;
    tm_now.tm_min = 0;
    tm_now.tm_sec = 0;

    now = mktime(&tm_now);
    sub->m_base_time = now * 1000000LL;

    /*! init base date */
    // refine tm
    tm_now.tm_year += 1900;
    tm_now.tm_mon += 1;

    // yyyymmdd format
    sub->m_base_date[0] = '0' + (tm_now.tm_year % 10000) / 1000;
    sub->m_base_date[1] = '0' + (tm_now.tm_year % 1000) / 100;
    sub->m_base_date[2] = '0' + (tm_now.tm_year % 100) / 10;
    sub->m_base_date[3] = '0' + (tm_now.tm_year % 10) / 1;

    sub->m_base_date[4] = '0' + (tm_now.tm_mon % 100) / 10;
    sub->m_base_date[5] = '0' + (tm_now.tm_mon % 10);

    sub->m_base_date[6] = '0' + (tm_now.tm_mday % 100) / 10;
    sub->m_base_date[7] = '0' + (tm_now.tm_mday % 10);
  }

  inline void init_quote(int i, const struct tm* pdate,
                         const char* const alias) {
    product_id id;
    product_id aid;
    prod_string(&aid, alias);
    roll_scheme::from_alias2(&id, alias, *pdate);
    static_cast<sub_class*>(this)->m_quotes.m_inst[i] = id;
    static_cast<sub_class*>(this)->m_quotes.m_aliases[i] = aid;
  }

  inline open_feature_type& get_feature(int i) {
    sub_class* sub = static_cast<sub_class*>(this);
    return sub->m_features[i];
  }
  inline const open_feature_type& get_feature(int i) const {
    const sub_class* sub = static_cast<const sub_class*>(this);
    return sub->m_features[i];
  }

  inline int get_next_feature(int cur_feature) const {
    return ((cur_feature + m_align_size - 1) / m_align_size) * m_align_size;
  }

  // return quote position,  m_quote_size means failed
  int has_quoted(const product_id& quote_id) const {
    int i;
    const sub_class* sub = static_cast<const sub_class*>(this);
    for (i = 0; i < m_quote_size; ++i) {
      //  printf("%s[%d] %lld \t%lld %s\n", quote_id.s8, i, quote_id.i64,
      //   sub->m_quotes.m_inst[i].i64, sub->m_quotes.m_inst[i].s8);
      if (quote_id.i64 == sub->m_quotes.m_inst[i].i64) break;
    }
    return i;
  }
  bool sanity_check(const msg_type& msg) {
    // bid > 0  && ask > bid
    if (msg.get_bid() <= 0 || msg.get_offer() <= msg.get_bid()) return false;

    if (msg.get_bid_quantity() <= 0 || msg.get_offer_quantity() <= 0)
      return false;
    return true;
  }

  template <class T = DCEQuotDataT,
            typename std::enable_if<
                std::is_same<T, DCEQuotDataT>::value>::type* = nullptr>
  bool convert_msg(const DCEQuotDataT& dce_msg, msg_type* pmsg,
                   int* pquote_fd) {
    const sub_class* sub = static_cast<const sub_class*>(this);

    // get pid
    product_id quote_id;
    prod_string(&quote_id, dce_msg.ContractID);

    // check quoted
    int quote_fd = has_quoted(quote_id);
    if (quote_fd == m_quote_size) return false;

    // convert msg
    msg_type& msg = *pmsg;
    // format hh:mm:ss.xxx
    msg.m_time_micro =
        sub->m_base_time + fast_totime(dce_msg.TradeDate, dce_msg.UpdateTime,
                                       this->m_base_date, this->m_base_time);
    msg.m_bid = dce_msg.BidPrice;
    msg.m_offer = dce_msg.AskPrice;
    msg.m_bid_quantity = dce_msg.BidQty;
    msg.m_offer_quantity = dce_msg.AskQty;
    msg.m_volume = dce_msg.LastMatchQty;
    msg.m_notional = dce_msg.Turnover;

    *pquote_fd = quote_fd;

    return true;
  }

  template <class T = ChinaL1Msg,
            typename std::enable_if<std::is_same<T, ChinaL1Msg>::value>::type* =
                nullptr>
  bool convert_msg(const ChinaL1Msg& msg_dep, msg_type* pmsg, int* pquote_fd) {
    lmice::product_id quote_id;
    quote_id.i64 = *((const int64_t*)msg_dep.m_inst);
    // prod_string(&quote_id, msg_dep.m_inst);
    // const sub_class* sub = static_cast<const sub_class*>(this);

    // check quoted
    int quote_fd = has_quoted(quote_id);
    if (quote_fd == m_quote_size) return false;

    *pquote_fd = quote_fd;

    msg_type& msg = *pmsg;
    // format hh:mm:ss.xxx
    msg.m_time_micro = msg_dep.get_time();
    msg.m_bid = msg_dep.get_bid();
    msg.m_offer = msg_dep.get_offer();
    msg.m_bid_quantity = msg_dep.get_bid_quantity();
    msg.m_offer_quantity = msg_dep.get_offer_quantity();
    msg.m_volume = msg_dep.get_volume();
    msg.m_notional = msg_dep.get_notional();

    return true;
  }
  template <class T = ChinaL1Msg,
            typename std::enable_if<std::is_same<T, ChinaL1Msg>::value>::type* =
                nullptr>
  int pre_proc_msg(const ChinaL1Msg& msg_dep, int qfd = 0) {
    (void)qfd;  // ignore it
    msg_type msg;
    int quote_fd;

    if (!convert_msg(msg_dep, &msg, &quote_fd)) return 0;

    int ret = is_prepare(msg, quote_fd);

    // printf("call pre_proc_msg ChinaL1Msg %d %d\t", ret, quote_fd);

    return ret;
  }

  template <class T = ChinaL1Msg,
            typename std::enable_if<std::is_same<T, ChinaL1Msg>::value>::type* =
                nullptr>
  void update(const ChinaL1Msg& msg_dep, int qfd = 0) {
    (void)qfd;  // ignore it
    int quote_fd;
    msg_type msg;
    lmice::product_id quote_id;
    if (!convert_msg(msg_dep, &msg, &quote_fd)) return;

    // handle_msg
    handle_msg(quote_fd, msg);
  }

  /*! 海通极速行情 初始化开仓模型
   * return :
   * 0: failed
   * other: success
   */
  template <class T = DCEQuotDataT,
            typename std::enable_if<
                std::is_same<T, DCEQuotDataT>::value>::type* = nullptr>
  int pre_proc_msg(const DCEQuotDataT& dce_msg, int qfd) {
    (void)qfd;  // ignore it
    const sub_class* sub = static_cast<const sub_class*>(this);

    int quote_fd;
    msg_type msg;
    lmice::product_id quote_id;
    if (!convert_msg(dce_msg, &msg, &quote_fd)) return 0;

    return is_prepare(msg, quote_fd);
  }

  template <class T = DCEQuotDataT,
            typename std::enable_if<std::is_same<T, ChinaL1Msg>::value>::type* =
                nullptr>
  void update(const DCEQuotDataT& dce_msg, int qfd = 0) {
    (void)qfd;  // ignore it

    int quote_fd;
    msg_type msg;
    lmice::product_id quote_id;
    if (!convert_msg(dce_msg, &msg, &quote_fd)) return;

    // handle_msg
    handle_msg(quote_fd, msg);
  }

  int is_prepare(const msg_type& msg, int quote_fd) {
    sub_class* sub = static_cast<sub_class*>(this);
    // sanity check the quotes
    if (!sanity_check(msg)) return 0;

    // update quotes
    update_quote(quote_fd, msg);

    // check contract's feature status
    for (int fd = 0; fd < m_contract_size; ++fd) {
      fm_contract<float_type>& cur_contract = sub->m_contracts[fd];

      for (int ffd = cur_contract.m_feature_range.m_feature_begin;
           ffd < cur_contract.m_feature_range.m_feature_end; ++ffd) {
        open_feature_type& feature = sub->m_features[ffd];
        if (!feature.is_ready()) {
          return 0;
        }
      }
    }

    return 1;
  }

  /*! 海通极速行情 开始交易 */
  void proc_msg(const DCEQuotDataT& dce_msg) {
    const sub_class* sub = static_cast<const sub_class*>(this);
    // get pid
    product_id quote_id;
    prod_string(&quote_id, dce_msg.ContractID);

    // check quoted
    int quote_fd = has_quoted(quote_id);
    if (quote_fd == m_quote_size) return;

    msg_type msg;
    // format hh:mm:ss.xxx
    msg.m_time_micro =
        sub->m_base_time + fast_totime(dce_msg.TradeDate, dce_msg.UpdateTime,
                                       this->m_base_date, this->m_base_time);
    msg.m_bid = dce_msg.BidPrice;
    msg.m_offer = dce_msg.AskPrice;
    msg.m_bid_quantity = dce_msg.BidQty;
    msg.m_offer_quantity = dce_msg.AskQty;
    msg.m_volume = dce_msg.LastMatchQty;
    msg.m_notional = dce_msg.Turnover;

    // handle_msg
    handle_msg(quote_fd, msg);
  }
  void proc_msg(const product_id& quote_id, const msg_type& msg) {
    // check quoted
    int quote_fd = has_quoted(quote_id);
    if (quote_fd < 0) return;

    handle_msg(quote_fd, msg);
  }
  template <class T, typename std::enable_if<
                         std::is_same<double, T>::value>::type* = nullptr>
  void calc_forecast(int quote_fd) {
    sub_class* sub = static_cast<sub_class*>(this);
    fm_contract<float_type>* cur_contract = &sub->m_contracts[quote_fd];
    int feature_begin = cur_contract->m_feature_range.m_feature_begin;
    int feature_end = cur_contract->m_feature_range.m_feature_end;
    const std::array<float_type, m_feature_size>& weight = sub->m_weights;
    std::array<float_type, m_feature_size>& signals = sub->m_signals;
    // calculate open signal
    float_type forecast = 0;

    for (int i = feature_begin; i < (feature_end + 7) / 8; ++i) {
      //      IACA_START

      __m256d xy = _mm256_mul_pd(*(__m256d*)&signals[i * 8 + 0],
                                 *(__m256d*)&weight[i * 8 + 0]);
      __m256d zw = _mm256_mul_pd(*(__m256d*)&signals[i * 8 + 4],
                                 *(__m256d*)&weight[i * 8 + 4]);
      __m256d temp = _mm256_hadd_pd(xy, zw);
      __m128d hi128 = _mm256_extractf128_pd(temp, 1);
      __m128d lo128 = _mm256_castpd256_pd128(temp);
      __m128d r1 = _mm_add_pd(lo128, hi128);

      forecast += r1[0] + r1[1];
    }
    //    IACA_END

    cur_contract->m_forecast = forecast;
    //    static int test_pos = 0;
    //    if (test_pos < 2) {
    //      test_pos++;
    //      printf("%16.14lf  \t%16.14lf %lf  %lf\n", cur_contract->m_forecast,
    //             signals[0], sub->m_features[0].get_signal(), weight[0]);
    //    }
  }
  template <class T, typename std::enable_if<
                         std::is_same<float, T>::value>::type* = nullptr>
  void calc_forecast(int quote_fd) {
    sub_class* sub = static_cast<sub_class*>(this);
    fm_contract<float_type>* cur_contract = &sub->m_contracts[quote_fd];
    int feature_begin = cur_contract->m_feature_range.m_feature_begin;
    int feature_end = cur_contract->m_feature_range.m_feature_end;
    const std::array<float_type, m_feature_size>& weight = sub->m_weights;
    std::array<float_type, m_feature_size>& signals = sub->m_signals;
    // calculate open signal
    cur_contract->m_forecast = 0;

    for (int i = feature_begin; i < (feature_end + 7) / 8; ++i) {
      // const float_type& forecast = sub->m_features[i].get_signal();

      const __m128 m1 = _mm_load_ps(&weight[i * 8]);
      const __m128 m2 = _mm_load_ps(&signals[i * 8]);
      __m128 r1 = _mm_mul_ps(m1, m2);

      const __m128 m3 = _mm_load_ps(&weight[i * 8 + 4]);
      const __m128 m4 = _mm_set_ps(&signals[i * 8 + 4]);

      __m128 r2 = _mm_mul_ps(m3, m4);

      //      _mm_storeu_ps(&signals[i * 8 + 0], r1);
      //      _mm_storeu_ps(&signals[i * 8 + 4], r2);

      const __m128 zero = _mm_setzero_ps();

      r1 = _mm_add_ps(r1, r2);
      r1 = _mm_hadd_ps(r1, zero);
      r1 = _mm_hadd_ps(r1, zero);
      _mm_store_ss(&cur_contract->m_forecast, r1);
    }
  }

  void handle_msg(int quote_fd, const msg_type& msg) {
    // sanity check the quotes
    if (!sanity_check(msg)) return;

    // update quotes
    update_quote(quote_fd, msg);
    //    static int g_delay = 100;
    //    static int g_count = 0;
    //    g_count++;
    //    if (g_count < g_delay) return;

    //    if (quote_fd == 1) {
    //      sub_class* sub = static_cast<sub_class*>(this);
    //      printf("found ni1709  %lf %lf\t", msg.m_bid, msg.m_offer);
    //      printf(" %lf %lf\n", sub->m_quotes.m_msgs[quote_fd][0].m_bid,
    //             sub->m_quotes.m_msgs[quote_fd][0].m_offer);
    //    }

    // handle self msg
    sub_class* sub = static_cast<sub_class*>(this);
    quote_feature_type quoted_feature = sub->m_quote_features;
    const feature_range& self_range = quoted_feature.m_self_range[quote_fd];

    // reset signal's value
    std::array<float_type, m_feature_size>& signals = sub->m_signals;
    memset(&signals[self_range.m_feature_begin], 0,
           sizeof(float_type) *
               (self_range.m_feature_end - self_range.m_feature_begin));

    // calculate self signal
    if (quoted_feature.m_handle_self_msg[quote_fd] != nullptr)
      quoted_feature.m_handle_self_msg[quote_fd](sub);
    //    for (int feature_fd = self_range.m_feature_begin;
    //         feature_fd < self_range.m_feature_end; ++feature_fd) {
    //      open_feature_type& feature = sub->m_features[feature_fd];
    //      feature.handle_self_msg();
    //    }

    // contract
    if (quote_fd < m_contract_size) {
      calc_forecast<float_type>(quote_fd);
    }
    //      for (int i = feature_begin; i < feature_end; ++i) {
    //        const float_type& weight = sub->m_weights[i];
    //        const float_type& forecast = sub->m_features[i].get_signal();
    //        float_type& signal = sub->m_signals[i];

    //        signal = weight * forecast;
    //      }

    // trade

    // handle ref msg
    auto ref_range = quoted_feature.m_ref_range[quote_fd];
    for (size_t i = 0; i < ref_range.size(); ++i) {
      if (quoted_feature.m_handle_other_msg[quote_fd][i] != nullptr)
        quoted_feature.m_handle_other_msg[quote_fd][i](sub);
      //      const feature_range& range = ref_range[i];

      //      for (int feature_fd = range.m_feature_begin;
      //           feature_fd < range.m_feature_end; ++feature_fd) {
      //        open_feature_type& feature = sub->m_features[feature_fd];
      //        feature.handle_other_msg();
      //      }
    }
  }

  void update_quote(int quote_fd, const msg_type& msg) {
    sub_class* sub = static_cast<sub_class*>(this);
    quote_msg_type& quote_msg = sub->m_quotes.m_msgs[quote_fd];
    // move cur to cur+1
    memcpy(&quote_msg[1], &quote_msg[0],
           sizeof(msg_type) * (m_quote_depth - 1));
    // update cur
    memcpy(&quote_msg[0], &msg, sizeof(msg_type));
  }
};

template <class sub_class>
struct fm_portfolio_data {
 private:
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_contract_size = fm_type_traits<sub_class>::m_contract_size,
    m_feature_size = fm_type_traits<sub_class>::m_feature_size
  };
  typedef typename fm_type_traits<sub_class>::float_type float_type;

  typedef typename fm_open_feature_type<float_type, m_quote_size, m_quote_depth,
                                        m_contract_size, m_feature_size>::type
      open_type;

 public:
  // static check contract size
  static_assert(m_contract_size <= m_quote_size, "contract must <= quote_size");

  //所有订阅的行情
  fm_chinal1_msg<float_type, m_quote_size, m_quote_depth> m_quotes;

  //所有交易的合约
  std::array<fm_contract<float_type>, m_contract_size> m_contracts;

  //所有子信号
  std::array<fm_open_feature<open_type>, m_feature_size> m_features;

  //行情对应的子信号
  fm_quote_feature<open_type> m_quote_features;

  // 子信号权重
  std::array<float_type, m_feature_size> m_weights forcepack(32);
  // 子信号值
  std::array<float_type, m_feature_size> m_signals forcepack(32);

  int64_t m_base_time;  // curdate hh(0) mm(0) ss(0)
  char m_base_date[8];  // yyyymmdd
};

template <class sub_class>
struct fm_portfolio_contract_handler {
 private:
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_contract_size = fm_type_traits<sub_class>::m_contract_size,
    m_feature_size = fm_type_traits<sub_class>::m_feature_size
  };
  typedef typename fm_type_traits<sub_class>::float_type float_type;
  typedef typename fm_open_feature_type<float_type, m_quote_size, m_quote_depth,
                                        m_contract_size, m_feature_size>::type
      open_type;
  typedef fm_open_feature<open_type> open_feature_type;

 public:
  inline float_type get_forecast(int contract_fd) const {
    const sub_class* t = static_cast<const sub_class*>(this);
    float_type value = 0;
    if (m_contract_size > contract_fd) {
      const fm_contract<float_type>& cur_contract = t->m_contracts[contract_fd];
      value = cur_contract.m_forecast;
      //      const feature_range& ct =
      //      t->m_contracts[contract_fd].m_feature_range; const
      //      std::array<float_type, m_feature_size>& sigs = t->m_signals; for
      //      (int i = ct.m_feature_begin; i < ct.m_feature_end; ++i)
      //        value += sigs[i];
    }
    return value;
  }

  void get_all_signal(int contract_fd, const float_type** sigs,
                      int* size) const {
    const sub_class* t = static_cast<const sub_class*>(this);
    if (m_contract_size > contract_fd) {
      const feature_range& ct = t->m_contracts[contract_fd].m_feature_range;
      const std::array<float_type, m_feature_size>& signals = t->m_signals;
      *sigs = &signals[ct.m_feature_begin];
      *size = ct.m_feature_end - ct.m_feature_begin;
    }
  }
  void get_all_weight(int contract_fd, const float_type** sigs,
                      int* size) const {
    const sub_class* t = static_cast<const sub_class*>(this);
    if (m_contract_size > contract_fd) {
      const feature_range& ct = t->m_contracts[contract_fd].m_feature_range;
      const std::array<float_type, m_feature_size>& weights = t->m_weights;
      *sigs = &weights[ct.m_feature_begin];
      *size = ct.m_feature_end - ct.m_feature_begin;
    }
  }

  std::string get_trading_instrument(int contract_fd) const {
    const sub_class* t = static_cast<const sub_class*>(this);
    std::string name;
    if (m_contract_size > contract_fd) {
      name = t->m_quotes.m_inst[contract_fd].s8;
    }
    return name;
  }
  std::vector<std::string> get_subscriptions(int contract_fd) const {
    const sub_class* t = static_cast<const sub_class*>(this);

    std::vector<std::string> vec;
    if (m_contract_size > contract_fd) {
      for (int i = 0; i < m_quote_size; ++i) {
        vec.push_back(t->m_quotes.m_inst[i].s8);
      }
    }
    return vec;
  }
  std::vector<std::string> get_subsignal_name(int contract_fd) const {
    const sub_class* t = static_cast<const sub_class*>(this);
    const std::array<product_id, m_quote_size>& aliases = t->m_quotes.m_aliases;
    std::vector<std::string> vec;
    if (m_contract_size > contract_fd) {
      const feature_range& ct = t->m_contracts[contract_fd].m_feature_range;

      for (int i = ct.m_feature_begin; i < ct.m_feature_end; ++i) {
        const open_feature_type& feature = t->m_features[i];
        // printf("%d  %d ", i, feature.m_type);
        {
          const fm_feature_data<open_type>& fdata =
              static_cast<const fm_feature_data<open_type>&>(feature);
          const char* type_names[] = {
              "OTHER_BS", "OTHER_BIB", "OTHER_DR",  "OTHER_TIB", "OTHER_TIB2",
              "SELF_BIB", "SELF_DR",   "SELF_OFIB", "SELF_TIB",  "SELF_TIB2"};
          std::string name;
          std::stringstream ss;

          ss << std::setw(2) << std::setfill('0') << i - ct.m_feature_begin
             << std::setw(0) << "_";
          ss << type_names[feature.m_type];

          int self_fd = fdata.m_self_fd;
          // printf("type %s %d ", type_names[feature.m_type], self_fd);
          ss << "_" << aliases[self_fd].s8;
          if (feature.m_type < SELF_BIB) {
            int other_fd = fdata.m_other_fd;

            ss << "_REF_" << aliases[other_fd].s8;
          }

          ss >> name;
          // printf("name %s\n", name.c_str());

          vec.push_back(name);
        }
      }
    }
    return vec;
  }
};

#define portfolio_param \
  float_type, quote_size, quote_depth, contract_size, feature_size
#define portfolio_type fm_portfolio<portfolio_param>

template <class float_type, int quote_size, int quote_depth, int contract_size,
          int feature_size>
struct fm_portfolio : public fm_portfolio_handler<portfolio_type>,
                      public fm_portfolio_data<portfolio_type>,
                      public fm_portfolio_contract_handler<portfolio_type> {
  static_assert(feature_size % 4 == 0, "feature size must be 4x");
} forcepack(128);
}  // namespace lmice

#endif  // FUTUREMODEL_INCLUDE_PORTFOLIO_H_
