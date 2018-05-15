/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include <stdio.h>

#include <iostream>
#include <vector>

#include "include/fastmath.h"
#include "include/portfolio.h"

// namespace lmice {
// template <class float_type, int quote_size, int quote_depth>
// union variant_feature {
//  fm_discrete_self_trade_imbalance_v2<float_type, quote_size, quote_depth>
//      dself_trade_imbalance_v2;
//  variant_feature(){};
//  ~variant_feature(){};
//};
// typedef variant_feature<double, 12, 2> feature_type;
//// template <class float_type>
//// struct demo : public fm_chinal1_msg_handler<demo<float_type>, 1>,
////              public fm_chinal1_msg_other_handler<demo<float_type>, 1> {
////  demo(fm_chinal1_msg<feature_type>& quotes) : m_quotes(quotes) {}
////  fm_chinal1_msg<feature_type>& m_quotes;
////};

// struct order {
//  int size;
//  int data[20];
//};
// struct feature_range {
//  uint32_t m_feature_begin;  //第一个子信号
//  uint32_t m_feature_end;    //最后一个子信号
//};

// template <class feature_type>
// struct quoted_feature {
//  enum { quote_size = fm_type_traits<feature_type>::m_quote_size };
//  std::array<feature_range, quote_size> m_self_range;
//  std::array<std::array<feature_range, 4>, quote_size> m_other_range;
//};

////期货合约对象
// struct çontract {
//  product_id m_inst;      //交易合约号
//  feature_range m_range;  //合约所有子信号
//  order m_order;          //交易报价单
//};

////投资组合
// struct portfolio {
//  enum {
//    quote_size = 12,
//    quote_depth = 2,
//    contract_size = 8,
//    feature_size = 80
//  };
//  typedef double float_type;
//  typedef variant_feature<double, 12, 2> feature_type;
//  fm_chinal1_msg<float_type, quote_size, quote_depth>
//      m_quotes;                                    //所有订阅的行情
//  quoted_feature<feature_type> m_quoted_features;  //行情对应的子信号
//  std::array<çontract, 8> m_çontracts;             //所有交易的合约
//  std::array<feature_type, 80> m_features;         //所有开仓子信号

//  void hand_self_quote(int quote_id) {
//    china_l1msg<float_type> msg;
//    const feature_range& fr = m_quoted_features.m_self_range[quote_id];
//    for (uint32_t i = fr.m_feature_begin; i < fr.m_feature_end; ++i) {
//      feature_type& feature = m_features[i];
//      // handle self msg
//      feature.dself_trade_imbalance_v2.handle_self_msg();
//    }
//  }
//  void hand_other_quote(int quote_id) {
//    china_l1msg<float_type> msg;
//    const std::array<feature_range, 4>& feature_ranges =
//        m_quoted_features.m_other_range[quote_id];
//    for (size_t range = 0; range < feature_ranges.size(); ++range) {
//      const feature_range& fr = feature_ranges[range];
//      for (uint32_t i = fr.m_feature_begin; i < fr.m_feature_end; ++i) {
//        feature_type& feature = m_features[i];
//        // handle other msg
//        feature.dself_trade_imbalance_v2.handle_self_msg();
//      }
//    }
//  }

//  void update() {
//    // update quote
//    int quote_pos = 0;

//    // handle self quote
//    hand_self_quote(quote_pos);

//    // calc open signal

//    // trade ...

//    // handle other quote
//    hand_other_quote(quote_pos);
//  }
//};
//}  // namespace lmice

inline struct tm fast_now() {
  struct tm tmnow;
  time_t tnow;
  time(&tnow);
  localtime_r(&tnow, &tmnow);
  return tmnow;
}

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
            ratio_mics_ = 30e6);                                               \
                                                                               \
    /* contract's other feature  ref-range end*/                               \
    ref_range.m_feature_end = feature_fd;                                      \
  }

template <class float_type, int quote_size, int quote_depth, int con_size,
          int fea_size>
struct forecaster {
  typedef lmice::fm_portfolio<float_type, quote_size, quote_depth, con_size,
                              fea_size>
      pf_type;

  pf_type* create() {
    pf_type* p = new pf_type;
    return p;
  }
};

struct cu1_0_forecaster2 {
  typedef double float_type;
  enum {
    m_quote_size = 6,
    m_quote_depth = 2,
    m_contract_size = 1,
    m_feature_size = 32
  };
  typedef lmice::fm_portfolio<float_type, m_quote_size, m_quote_depth,
                              m_contract_size, m_feature_size>
      pf_type;
  pf_type* m_po;
  std::string get_trading_instrument() const {
    return m_po->get_trading_instrument(0);
  }
  std::vector<std::string> get_subsignal_name() const {
    return m_po->get_subsignal_name(0);
  }

  std::vector<std::string> get_subscriptions() const {
    return m_po->get_subscriptions(0);
  }

  void update(const ChinaL1Msg& msg_dep) { m_po->update(msg_dep); }
  float_type get_forecast() { return m_po->get_forecast(0); }
  void get_all_signal(const float_type** sigs, int* size) const {
    m_po->get_all_signal(0, sigs, size);
  }
  void get_all_weight(const float** sigs, int* size) const {
    // m_po->get_all_weight(0, sigs, size);
  }
  cu1_0_forecaster2() { m_po = new pf_type; }
  ~cu1_0_forecaster2() { delete m_po; }
  void init(struct tm date) {
    using namespace lmice;
    pf_type& po = *m_po;

    po.init(date);

    /*! init quote */
    int quote_fd = 0;
    po.init_quote(quote_fd++, &date, "cu01");
    po.init_quote(quote_fd++, &date, "ni01");
    po.init_quote(quote_fd++, &date, "zn01");
    po.init_quote(quote_fd++, &date, "al01");
    po.init_quote(quote_fd++, &date, "pb01");
    po.init_quote(quote_fd++, &date, "sn01");

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
          .init(
              feature_type_ = OTHER_TIB,
              feature_name_ = "ChinaL1DiscreteOtherTradeImbalance_ni01_Ratio30",
              trading_fd_ = trading_fd, contract_size_ = contract_size,
              tick_size_ = tick_size, reference_fd_ = reference_fd,
              ratio_mics_ = 30e6);

      // contract's other feature  ref-range end
      ref_range.m_feature_end = feature_fd;

      // contract's next group other-feature ...
    }

    INIT_CU1_OTHER(zn01);
    INIT_CU1_OTHER(al01);
    INIT_CU1_OTHER(pb01);
    INIT_CU1_OTHER(sn01);

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
  }
};

int main() {
  using namespace lmice;

  struct tm date = fast_now();
  date.tm_year = 2017 - 1900;
  date.tm_mon = 5 - 1;
  date.tm_mday = 22;

  typedef double float_type;

  lmice::fm_portfolio<float_type, 6, 2, 1, 64> po;

  po.init(date);
  /*! init quote */
  int quote_fd = 0;
  po.init_quote(quote_fd++, &date, "cu01");
  po.init_quote(quote_fd++, &date, "ni01");
  po.init_quote(quote_fd++, &date, "zn01");
  po.init_quote(quote_fd++, &date, "al01");
  po.init_quote(quote_fd++, &date, "pb01");
  po.init_quote(quote_fd++, &date, "sn01");

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
              ratio_mics_ = 30e6);

    // contract's other feature  ref-range end
    ref_range.m_feature_end = feature_fd;

    // contract's next group other-feature ...
  }

  INIT_CU1_OTHER(zn01);
  INIT_CU1_OTHER(al01);
  INIT_CU1_OTHER(pb01);
  INIT_CU1_OTHER(sn01);

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

  /*! verify */
  for (size_t i = 0; i < po.m_quotes.m_inst.size(); ++i) {
    printf("inst %s\n", po.m_quotes.m_inst[i].s8);
  }

  lmice::china_l1msg<float_type> msg;
  lmice::product_id id("cu1806");

  po.proc_msg(id, msg);
  printf("sizeof portfolio %lu time %lld\n", sizeof(po), po.m_base_time);

  for (size_t i = 0; i < po.m_quote_features.m_ref_range.size(); ++i) {
    printf("%s ", po.m_quotes.m_inst[i].s8);
    for (size_t j = 0; j < po.m_quote_features.m_ref_range[i].size(); ++j) {
      const feature_range& range = po.m_quote_features.m_ref_range[i][j];
      printf("%lu-%lu: %d -- %d\t", i, j, range.m_feature_begin,
             range.m_feature_end);
    }
    printf("%d -- %d\n", po.m_quote_features.m_self_range[i].m_feature_begin,
           po.m_quote_features.m_self_range[i].m_feature_end);
  }
  /*! 1 初始化执行环境 */
  /*! 1.1 清除上一次运行状态 */
  /*! 1.2 初始化执行环境 */
  /*! 1.2.1 创建共享存储区域 */
  /*! 1.2.2 初始化投资组合模型 */
  //  lmice::portfolio pf;
  //  lmice::demo<double> dm(pf.m_quotes);
  //  printf("portfolio=%lu\n ", sizeof(pf));
  //  printf("demo size=%lu\n", sizeof(dm));
  std::vector<std::string> signames = po.get_subsignal_name(0);
  printf("contract 0 size%lu\n", signames.size());
  for (size_t i = 0; i < signames.size(); ++i) {
    printf("%lu\t%s\n", i, signames[i].c_str());
  }

  return 0;
}
