#include "iaca/iacaMarks.h"

#include "include/portfolio.h"

#include "include/forecasterimpl.h"

#define INIT_CU1_OTHER(inst_name)                                              \
  {                                                                            \
    /* contract's first group other-feature*/                                  \
    int reference_fd = po.get_quote_fd(#inst_name);                            \
    contract_size = roll_scheme::get_constract_size(#inst_name);               \
    tick_size = roll_scheme::get_ticksize(#inst_name);                         \
    printf("ref " #inst_name " quotefd %d\n", reference_fd);                   \
                                                                               \
    /*new ref range*/                                                          \
    feature_range& ref_range = po.get_next_ref_range(reference_fd);            \
                                                                               \
    /* contract's other feature ref-range begin */                             \
    ref_range.m_feature_begin = feature_fd;                                    \
                                                                               \
    /* contract's other reference */                                           \
    po.get_feature(feature_fd++)                                               \
        .init(feature_type_ = OTHER_BS,                                        \
              feature_name_ = "L1DisOtherBestSince_" #inst_name "_Ratio1",     \
              trading_fd_ = trading_fd, reference_fd_ = reference_fd,          \
              contract_size_ = contract_size, tick_size_ = tick_size,          \
              ratio_mics_ = 1e6, use_diff_ = true, corr_negative_ = false);    \
    po.get_feature(feature_fd++)                                               \
        .init(feature_type_ = OTHER_DR,                                        \
              feature_name_ = "L1DisOtherDecayReturn_" #inst_name "_Decay60",  \
              trading_fd_ = trading_fd, contract_size_ = contract_size,        \
              tick_size_ = tick_size, reference_fd_ = reference_fd,            \
              book_decay_ = 60e6);                                             \
    po.get_feature(feature_fd++)                                               \
        .init(feature_type_ = OTHER_DR,                                        \
              feature_name_ = "L1DisOtherDecayReturn_" #inst_name "_Decay10",  \
              trading_fd_ = trading_fd, contract_size_ = contract_size,        \
              tick_size_ = tick_size, reference_fd_ = reference_fd,            \
              book_decay_ = 10e6);                                             \
    po.get_feature(feature_fd++)                                               \
        .init(feature_type_ = OTHER_DR,                                        \
              feature_name_ = "L1DisOtherDecayReturn_" #inst_name "_Decay1",   \
              trading_fd_ = trading_fd, contract_size_ = contract_size,        \
              tick_size_ = tick_size, reference_fd_ = reference_fd,            \
              book_decay_ = 1e6);                                              \
    po.get_feature(feature_fd++)                                               \
        .init(                                                                 \
            feature_type_ = OTHER_TIB,                                         \
            feature_name_ = "L1DisOtherTradeImbalance_" #inst_name "_Ratio30", \
            trading_fd_ = trading_fd, contract_size_ = contract_size,          \
            tick_size_ = tick_size, reference_fd_ = reference_fd,              \
            ratio_mics_ = 30e6, use_diff_ = true, corr_negative_ = false);     \
                                                                               \
    /* contract's other feature  ref-range end*/                               \
    ref_range.m_feature_end = feature_fd;                                      \
  }

enum {
  m_quote_size = 6,
  m_quote_depth = 2,
  m_contract_size = 1,
  m_feature_size = 32
};
typedef lmice::fm_portfolio<cu1_0_forecaster::float_type, m_quote_size,
                            m_quote_depth, m_contract_size, m_feature_size>
    pf_type;

// 投资组合
typedef cu1_0_forecaster::float_type float_type;

typedef typename lmice::fm_open_feature_type<float_type, m_quote_size,
                                             m_quote_depth, m_contract_size,
                                             m_feature_size>::type open_type;
typedef lmice::fm_open_feature<open_type> open_feature_type;

typedef lmice::fm_discrete_other_best_since<open_type> obs_type;
typedef lmice::fm_discrete_other_book_imbalance<open_type> obi_type;
typedef lmice::fm_discrete_other_decaying_return<open_type> odr_type;
typedef lmice::fm_discrete_other_trade_imbalance<open_type> oti_type;
typedef lmice::fm_discrete_other_trade_imbalance_v2<open_type> oti2_type;

typedef lmice::fm_discrete_self_book_imbalance<open_type> sbi_type;
typedef lmice::fm_discrete_self_decaying_return<open_type> sdr_type;
typedef lmice::fm_discrete_self_orderflow_imbalance<open_type> soi_type;
typedef lmice::fm_discrete_self_trade_imbalance<open_type> sti_type;
typedef lmice::fm_discrete_self_trade_imbalance_v2<open_type> sti2_type;

template <class this_type>
inline void handle_self_msg(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->handle_self_msg();
}
template <class this_type>
inline void handle_other_msg(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->handle_other_msg();
}
template <class this_type>
inline void prepare(open_feature_type* self) {
  this_type* pthis = static_cast<this_type*>(self);
  pthis->prepare();
}

typedef lmice::fast_ema<cu1_0_forecaster::float_type> fast_ema;

fast_ema g_forecast_ema;
int64_t g_time_micro;
bool g_ema_inited = false;

// pf_type* g_portfolio;

void handle_self_msg_st(pf_type* g_portfolio) {
  using namespace lmice;

  // for (int i = 0; i < 100; ++i) {
  // IACA_START
  handle_self_msg<sbi_type>(&(g_portfolio->get_feature(0)));
  handle_self_msg<sti2_type>(&(g_portfolio->get_feature(1)));
  handle_self_msg<sdr_type>(&(g_portfolio->get_feature(2)));
  handle_self_msg<sdr_type>(&(g_portfolio->get_feature(3)));
  handle_self_msg<sdr_type>(&(g_portfolio->get_feature(4)));

  // other ni01
  handle_self_msg<obs_type>(&(g_portfolio->get_feature(5)));
  handle_self_msg<obs_type>(&(g_portfolio->get_feature(10)));
  handle_self_msg<obs_type>(&(g_portfolio->get_feature(15)));
  handle_self_msg<obs_type>(&(g_portfolio->get_feature(20)));
  handle_self_msg<obs_type>(&(g_portfolio->get_feature(25)));

  handle_self_msg<odr_type>(&(g_portfolio->get_feature(6)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(7)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(8)));
  handle_self_msg<oti_type>(&(g_portfolio->get_feature(9)));

  // other zn01

  handle_self_msg<odr_type>(&(g_portfolio->get_feature(11)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(12)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(13)));
  handle_self_msg<oti_type>(&(g_portfolio->get_feature(14)));

  // other al01

  handle_self_msg<odr_type>(&(g_portfolio->get_feature(16)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(17)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(18)));
  handle_self_msg<oti_type>(&(g_portfolio->get_feature(19)));

  // other pb01

  handle_self_msg<odr_type>(&(g_portfolio->get_feature(21)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(22)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(23)));
  handle_self_msg<oti_type>(&(g_portfolio->get_feature(24)));

  // other sn01

  handle_self_msg<odr_type>(&(g_portfolio->get_feature(26)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(27)));
  handle_self_msg<odr_type>(&(g_portfolio->get_feature(28)));
  handle_self_msg<oti_type>(&(g_portfolio->get_feature(29)));
  //}
  // IACA_END
}

void handle_other_msg_ni01(pf_type* g_portfolio) {
  // other ni01
  handle_other_msg<obs_type>(&(g_portfolio->get_feature(5)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(6)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(7)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(8)));
  handle_other_msg<oti_type>(&(g_portfolio->get_feature(9)));
}
void handle_other_msg_zn01(pf_type* g_portfolio) {
  // other zn01
  handle_other_msg<obs_type>(&(g_portfolio->get_feature(10)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(11)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(12)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(13)));
  handle_other_msg<oti_type>(&(g_portfolio->get_feature(14)));
}
void handle_other_msg_al01(pf_type* g_portfolio) {
  // other ni01
  handle_other_msg<obs_type>(&(g_portfolio->get_feature(15)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(16)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(17)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(18)));
  handle_other_msg<oti_type>(&(g_portfolio->get_feature(19)));
}
void handle_other_msg_pb01(pf_type* g_portfolio) {
  // other ni01
  handle_other_msg<obs_type>(&(g_portfolio->get_feature(20)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(21)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(22)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(23)));
  handle_other_msg<oti_type>(&(g_portfolio->get_feature(24)));
}
void handle_other_msg_sn01(pf_type* g_portfolio) {
  // other ni01
  handle_other_msg<obs_type>(&(g_portfolio->get_feature(25)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(26)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(27)));
  handle_other_msg<odr_type>(&(g_portfolio->get_feature(28)));
  handle_other_msg<oti_type>(&(g_portfolio->get_feature(29)));
}

std::string cu1_0_forecaster::get_trading_instrument() const {
  pf_type* m_po = (pf_type*)m_pimpl;
  return m_po->get_trading_instrument(0);
}
std::vector<std::string> cu1_0_forecaster::get_subsignal_name() const {
  pf_type* m_po = (pf_type*)m_pimpl;
  return m_po->get_subsignal_name(0);
}

std::vector<std::string> cu1_0_forecaster::get_subscriptions() const {
  pf_type* m_po = (pf_type*)m_pimpl;
  return m_po->get_subscriptions(0);
}

int cu1_0_forecaster::pre_proc_msg(const ChinaL1Msg& msg_dep) {
  pf_type* m_po = (pf_type*)m_pimpl;
  return m_po->pre_proc_msg(msg_dep, 0);
}

void cu1_0_forecaster::update(const ChinaL1Msg& msg_dep) {
  pf_type* m_po = (pf_type*)m_pimpl;
  m_po->update(msg_dep);
  // g_time_micro = msg_dep.get_time();
}
cu1_0_forecaster::float_type cu1_0_forecaster::get_forecast() {
  pf_type* m_po = (pf_type*)m_pimpl;
  float_type value = m_po->get_forecast(0);
  //  if (g_ema_inited == false) {
  //    g_ema_inited = true;
  //    g_forecast_ema.init(value, g_time_micro);
  //  }
  //  g_forecast_ema.update(value, g_time_micro);
  return value;
}
cu1_0_forecaster::float_type cu1_0_forecaster::get_forecast1() {
  return g_forecast_ema.get_value();
}

void cu1_0_forecaster::get_all_signal(const float** sigs, int* size) const {
  pf_type* m_po = (pf_type*)m_pimpl;
  const float_type* pos;
  m_po->get_all_signal(0, &pos, size);

  float* p = (float*)m_buffer;
  for (int i = 0; i < *size; ++i) {
    p[i] = pos[i];
  }
  *sigs = p;
}

void cu1_0_forecaster::get_all_weight(const float** sigs, int* size) const {
  pf_type* m_po = (pf_type*)m_pimpl;

  const float_type* pos;
  m_po->get_all_weight(0, &pos, size);

  float* p = (float*)m_buffer + 512;
  for (int i = 0; i < *size; ++i) {
    p[i] = pos[i];
  }
  *sigs = &p[512];
}
cu1_0_forecaster::cu1_0_forecaster() {
  m_pimpl = new pf_type;
  // g_portfolio = new pf_type;
  // m_pimpl = g_portfolio;
  m_buffer = malloc(sizeof(float) * 1024);
  g_forecast_ema.init(1.0 / 500000);
}

cu1_0_forecaster::~cu1_0_forecaster() {
  delete (pf_type*)m_pimpl;
  free(m_buffer);
}

void cu1_0_forecaster::init(struct tm date) {
  using namespace lmice;
  pf_type& po = *(pf_type*)m_pimpl;
  // pf_type* pportfolio = (pf_type*)m_pimpl;

  po.init(date);

  char data_str[16] = {0};
  data_str[0] = '0' + (date.tm_hour / 10);
  data_str[1] = '0' + (date.tm_hour % 10);
  data_str[2] = '-';
  data_str[3] = '0' + (date.tm_min / 10);
  data_str[4] = '0' + (date.tm_min % 10);
  data_str[5] = '-';
  data_str[6] = '0' + (date.tm_sec / 10);
  data_str[7] = '0' + (date.tm_sec % 10);
  data_str[8] = '.';
  data_str[9] = '0';
  data_str[10] = '0';
  data_str[11] = '0';

  g_forecast_ema.init(0, fast_totime(data_str));

  /*! init quote */
  int quote_fd = 0;
  po.init_quote(quote_fd++, &date, "cu01");
  po.init_quote(quote_fd++, &date, "ni01");
  po.init_quote(quote_fd++, &date, "zn01");
  po.init_quote(quote_fd++, &date, "al01");
  po.init_quote(quote_fd++, &date, "pb01");
  po.init_quote(quote_fd++, &date, "sn01");

  for (int i = 0; i < 6; ++i) {
    printf("init_quote %s  %s  %lld\n", po.m_quotes.m_inst[i].s8,
           po.m_quotes.m_aliases[i].s8, po.m_quotes.m_inst[i].i64);
  }

  /*! init features-for each contract */
  int contract_fd = 0;
  int feature_fd = 0;

  int trading_fd = po.get_quote_fd("cu01");
  int contract_size = roll_scheme::get_constract_size("cu01");
  int tick_size = roll_scheme::get_ticksize("cu01");

  // contract 0
  fm_contract<float_type>& contract = po.m_contracts[contract_fd];
  feature_range& self_range = po.m_quote_features.m_self_range[contract_fd];

  // contract's feature range begin
  contract.m_feature_range.m_feature_begin = feature_fd;

  {
    // contract's  self-feature ...
    po.get_feature(feature_fd++)
        .init(feature_type_ = SELF_BIB,
              feature_name_ = "ChinaL1DiscreteSelfBookImbalanceFeature",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size);

    po.get_feature(feature_fd++)
        .init(feature_type_ = SELF_TIB2,
              feature_name_ = "ChinaL1DiscreteSelfTradeImbalanceV2_Decay30",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, book_decay_ = 30e6);
    po.get_feature(feature_fd++)
        .init(feature_type_ = SELF_DR,
              feature_name_ = "ChinaL1DiscreteSelfDecayingReturn_Decay60",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, book_decay_ = 60e6);

    po.get_feature(feature_fd++)
        .init(feature_type_ = SELF_DR,
              feature_name_ = "ChinaL1DiscreteSelfDecayingReturn_Decay10",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, book_decay_ = 10e6);

    po.get_feature(feature_fd++)
        .init(feature_type_ = SELF_DR,
              feature_name_ = "ChinaL1DiscreteSelfDecayingReturn_Decay1",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, book_decay_ = 1e6);
  }

  {
    // contract's first group other-feature
    int reference_fd = po.get_quote_fd("ni01");
    contract_size = roll_scheme::get_constract_size("ni01");
    tick_size = roll_scheme::get_ticksize("ni01");
    printf("ref ni01 quotefd %d\n", reference_fd);

    // new ref range
    feature_range& ref_range = po.get_next_ref_range(reference_fd);

    // contract's other feature ref-range begin
    ref_range.m_feature_begin = feature_fd;

    // contract's other reference
    po.get_feature(feature_fd++)
        .init(feature_type_ = OTHER_BS,
              feature_name_ = "ChinaL1DiscreteOtherBestSince_ni01_Ratio1",
              trading_fd_ = trading_fd, reference_fd_ = reference_fd,
              contract_size_ = contract_size, tick_size_ = tick_size,
              ratio_mics_ = 1e6, use_diff_ = true, corr_negative_ = false);
    po.get_feature(feature_fd++)
        .init(feature_type_ = OTHER_DR,
              feature_name_ = "ChinaL1DiscreteOtherDecayReturn_ni01_Decay60",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, reference_fd_ = reference_fd,
              book_decay_ = 60e6);
    po.get_feature(feature_fd++)
        .init(feature_type_ = OTHER_DR,
              feature_name_ = "ChinaL1DiscreteOtherDecayReturn_ni01_Decay10",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, reference_fd_ = reference_fd,
              book_decay_ = 10e6);
    po.get_feature(feature_fd++)
        .init(feature_type_ = OTHER_DR,
              feature_name_ = "ChinaL1DiscreteOtherDecayReturn_ni01_Decay1",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, reference_fd_ = reference_fd,
              book_decay_ = 1e6);
    po.get_feature(feature_fd++)
        .init(feature_type_ = OTHER_TIB,
              feature_name_ = "ChinaL1DiscreteOtherTradeImbalance_ni01_Ratio30",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, reference_fd_ = reference_fd,
              ratio_mics_ = 30e6, use_diff_ = true, corr_negative_ = false);

    // contract's other feature  ref-range end
    ref_range.m_feature_end = feature_fd;

    // contract's next group other-feature ...
  }

  INIT_CU1_OTHER(zn01);
  INIT_CU1_OTHER(al01);
  INIT_CU1_OTHER(pb01);
  INIT_CU1_OTHER(sn01);

  /*! init feature's portfolio pointer, and feature fd */
  for (feature_fd = 0; feature_fd < m_feature_size; ++feature_fd) {
    po.get_feature(feature_fd).init_portfolio(&po, feature_fd);
  }
#if 1
  /*! init function pointer for quoted feature */
  po.m_quote_features.m_handle_self_msg[0] = handle_self_msg_st;

  int reference_fd = po.get_quote_fd("ni01");

  /*! handle other msg */

  po.m_quote_features.m_handle_other_msg[reference_fd][0] =
      handle_other_msg_ni01;

  reference_fd = po.get_quote_fd("zn01");

  /*! handle other msg */
  //  struct handle_other_msg_zn01 {
  //    void operator()() {
  //      // other zn01
  //      handle_other_msg<obs_type>(&(g_portfolio->get_feature(10)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(11)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(12)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(13)));
  //      handle_other_msg<oti_type>(&(g_portfolio->get_feature(14)));
  //    }
  //  };
  po.m_quote_features.m_handle_other_msg[reference_fd][0] =
      handle_other_msg_zn01;

  reference_fd = po.get_quote_fd("al01");

  /*! handle other msg */
  //  struct handle_other_msg_al01 {
  //    void operator()() {
  //      // other al01
  //      handle_other_msg<obs_type>(&(g_portfolio->get_feature(15)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(16)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(17)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(18)));
  //      handle_other_msg<oti_type>(&(g_portfolio->get_feature(19)));
  //    }
  //  };
  po.m_quote_features.m_handle_other_msg[reference_fd][0] =
      handle_other_msg_al01;

  reference_fd = po.get_quote_fd("pb01");

  /*! handle other msg */
  //  struct handle_other_msg_pb01 {
  //    void operator()() {
  //      // other pb01
  //      handle_other_msg<obs_type>(&(g_portfolio->get_feature(20)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(21)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(22)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(23)));
  //      handle_other_msg<oti_type>(&(g_portfolio->get_feature(24)));
  //    }
  //  };
  po.m_quote_features.m_handle_other_msg[reference_fd][0] =
      handle_other_msg_pb01;

  reference_fd = po.get_quote_fd("sn01");

  /*! handle other msg */
  //  struct handle_other_msg_sn01 {
  //    void operator()() {
  //      // other sn01
  //      handle_other_msg<obs_type>(&(g_portfolio->get_feature(25)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(26)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(27)));
  //      handle_other_msg<odr_type>(&(g_portfolio->get_feature(28)));
  //      handle_other_msg<oti_type>(&(g_portfolio->get_feature(29)));
  //    }
  //  };
  po.m_quote_features.m_handle_other_msg[reference_fd][0] =
      handle_other_msg_sn01;
#endif

  // contract's feature range end
  contract.m_feature_range.m_feature_end = feature_fd;

  // contract's weight
  std::array<float_type, 30> weights = {
      {/*cu01*/
       0.055342, 0.021088, 0.105038, 0.071471, 0.015296,
       /*ni01*/
       0.032747, 0.024476, 0.024633, 0.013012, 0.000148,
       /*zn01*/
       0.043439, 0.039653, 0.049075, 0.025577, 0.000251,
       /*al01*/
       0.029389, 0.016672, 0.014483, 0.010211, 0.000087,
       /*pb01*/
       0.024055, 0.048986, 0.008690, 0.007171, 0.000060,
       /*sn01*/
       0.000295, -0.018380, -0.010077, 0.002318, 0.000001}};
  memcpy(&po.m_weights[contract.m_feature_range.m_feature_begin], &weights[0],
         30 * sizeof(float_type));

  // contract's order
  contract.m_order.id = 0;
  // contract's cancel
  contract.m_calcel.id = 0;

  // quote_feature self range
  self_range.m_feature_begin = contract.m_feature_range.m_feature_begin;
  self_range.m_feature_end = contract.m_feature_range.m_feature_end;

  // next feature fd
  feature_fd = po.get_next_feature(feature_fd);
  // contract_fd + xx;
  // feature_fd align
  // feature_fd + xx;
  contract = po.m_contracts[contract_fd];
  self_range = po.m_quote_features.m_self_range[contract_fd];

  for (int i = 0; i < 6; ++i) {
    printf("init_quote %s  %s\n", po.m_quotes.m_inst[i].s8,
           po.m_quotes.m_aliases[i].s8);
  }
}
