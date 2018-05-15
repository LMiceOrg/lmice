#ifndef OPENFEATURE_H
#define OPENFEATURE_H

#include <type_traits>

#define BOOST_PARAMETER_MAX_ARITY 13
#include <boost/parameter.hpp>

#include "include/chinal1msg.h"
#include "include/featurebase.h"
namespace lmice {
template <class sub_class>
struct fm_open_feature;
}
#include "include/discrete_self_book_imbalance.h"
#include "include/discrete_self_decaying_return.h"
#include "include/discrete_self_orderflow_imbalance.h"
#include "include/discrete_self_trade_imbalance.h"
#include "include/discrete_self_trade_imbalance_v2.h"

#include "include/discrete_other_best_since.h"
#include "include/discrete_other_book_imbalance.h"
#include "include/discrete_other_decaying_return.h"
#include "include/discrete_other_trade_imbalance.h"
#include "include/discrete_other_trade_imbalance_v2.h"

namespace lmice {

enum variant_feature_enum {
  OTHER_BS,
  OTHER_BIB,
  OTHER_DR,
  OTHER_TIB,
  OTHER_TIB2,
  SELF_BIB,
  SELF_DR,
  SELF_OFIB,
  SELF_TIB,
  SELF_TIB2
};

//关键字标签类型(keyword tag type)
namespace feature_tag {
struct feature_type;
struct feature_name;
struct weight;
struct trading_alias;
struct date;
struct book_decay;
/*other */
struct reference_alias;
struct cutoff_time;
struct ratio_mics;
struct use_diff;
struct corr_negative;

struct trading_fd;
struct reference_fd;

struct contract_size;
struct tick_size;
}  // namespace feature_tag

// 无名的名字空间
namespace {
// A reference to the keyword object 对关键字对象的引用
boost::parameter::keyword<feature_tag::feature_type>& feature_type_ =
    boost::parameter::keyword<feature_tag::feature_type>::get();

boost::parameter::keyword<feature_tag::feature_name>& feature_name_ =
    boost::parameter::keyword<feature_tag::feature_name>::get();

boost::parameter::keyword<feature_tag::weight>& weight_ =
    boost::parameter::keyword<feature_tag::weight>::get();

boost::parameter::keyword<feature_tag::trading_alias>& trading_alias_ =
    boost::parameter::keyword<feature_tag::trading_alias>::get();

boost::parameter::keyword<feature_tag::date>& date_ =
    boost::parameter::keyword<feature_tag::date>::get();

boost::parameter::keyword<feature_tag::book_decay>& book_decay_ =
    boost::parameter::keyword<feature_tag::book_decay>::get();

boost::parameter::keyword<feature_tag::reference_alias>& reference_alias_ =
    boost::parameter::keyword<feature_tag::reference_alias>::get();

boost::parameter::keyword<feature_tag::cutoff_time>& cutoff_time_ =
    boost::parameter::keyword<feature_tag::cutoff_time>::get();

boost::parameter::keyword<feature_tag::ratio_mics>& ratio_mics_ =
    boost::parameter::keyword<feature_tag::ratio_mics>::get();

boost::parameter::keyword<feature_tag::use_diff>& use_diff_ =
    boost::parameter::keyword<feature_tag::use_diff>::get();

boost::parameter::keyword<feature_tag::corr_negative>& corr_negative_ =
    boost::parameter::keyword<feature_tag::corr_negative>::get();

boost::parameter::keyword<feature_tag::trading_fd>& trading_fd_ =
    boost::parameter::keyword<feature_tag::trading_fd>::get();
boost::parameter::keyword<feature_tag::reference_fd>& reference_fd_ =
    boost::parameter::keyword<feature_tag::reference_fd>::get();

boost::parameter::keyword<feature_tag::contract_size>& contract_size_ =
    boost::parameter::keyword<feature_tag::contract_size>::get();
boost::parameter::keyword<feature_tag::tick_size>& tick_size_ =
    boost::parameter::keyword<feature_tag::tick_size>::get();
}  // namespace

/** open feature type */
template <typename ft_type, int quote_size, int quote_depth, int contract_size,
          int feature_size>
struct fm_open_feature_type {
  typedef fm_open_feature_type<ft_type, quote_size, quote_depth, contract_size,
                               feature_size>
      type;
  typedef ft_type float_type;
  enum { m_quote_size = quote_size, m_quote_depth = quote_depth };
};

/** open feature */
template <class sub_class>
struct fm_open_feature
    : public fm_feature_data<sub_class>,
      public fm_feature_chinal1_handler<fm_open_feature<sub_class>, sub_class> {
 private:
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_contractmodel_size = fm_type_traits<sub_class>::m_contract_size,
    m_feature_size = fm_type_traits<sub_class>::m_feature_size
  };
  typedef typename fm_type_traits<sub_class>::float_type float_type;
  typedef fast_ema<float_type> fast_ema;
  typedef fm_chinal1_msg<float_type, m_quote_size, m_quote_depth> quotes_type;
  typedef fm_open_feature<sub_class> this_type;
  typedef fm_feature_data<sub_class> data_type;
  typedef
      typename fm_open_feature_type<float_type, m_quote_size, m_quote_depth,
                                    m_contractmodel_size, m_feature_size>::type
          open_type;
  typedef fm_open_feature<open_type> sub_type;
  typedef fm_portfolio<float_type, m_quote_size, m_quote_depth,
                       m_contractmodel_size, m_feature_size>
      portfolio_type;

  typedef fm_discrete_other_best_since<open_type> obs_type;
  typedef fm_discrete_other_book_imbalance<open_type> obi_type;
  typedef fm_discrete_other_decaying_return<open_type> odr_type;
  typedef fm_discrete_other_trade_imbalance<open_type> oti_type;
  typedef fm_discrete_other_trade_imbalance_v2<open_type> oti2_type;

  typedef fm_discrete_self_book_imbalance<open_type> sbi_type;
  typedef fm_discrete_self_decaying_return<open_type> sdr_type;
  typedef fm_discrete_self_orderflow_imbalance<open_type> soi_type;
  typedef fm_discrete_self_trade_imbalance<open_type> sti_type;
  typedef fm_discrete_self_trade_imbalance_v2<open_type> sti2_type;

  typedef std::integral_constant<int, OTHER_BS> obs_const;

 public:
  USING_FEATURE_DATA(data_type)

  void handle_self_msg() {
    switch (m_type) {
      case OTHER_BS:
        static_cast<obs_type*>(this)->handle_self_msg();
        break;
      case OTHER_BIB:
        static_cast<obi_type*>(this)->handle_self_msg();
        break;
      case OTHER_DR:
        static_cast<odr_type*>(this)->handle_self_msg();
        break;
      case OTHER_TIB:
        static_cast<oti_type*>(this)->handle_self_msg();
        break;
      case OTHER_TIB2:
        static_cast<oti2_type*>(this)->handle_self_msg();
        break;

      case SELF_BIB:
        static_cast<sbi_type*>(this)->handle_self_msg();
        break;
      case SELF_DR:
        static_cast<sdr_type*>(this)->handle_self_msg();
        break;
      case SELF_OFIB:
        static_cast<soi_type*>(this)->handle_self_msg();
        break;
      case SELF_TIB:
        static_cast<sti_type*>(this)->handle_self_msg();
        break;
      case SELF_TIB2:
        static_cast<sti2_type*>(this)->handle_self_msg();
        break;
    }
  }
  void handle_other_msg() {
    switch (m_type) {
      case OTHER_BS:
        static_cast<obs_type*>(this)->handle_other_msg();
        break;
      case OTHER_BIB:
        static_cast<obi_type*>(this)->handle_other_msg();
        break;
      case OTHER_DR:
        static_cast<odr_type*>(this)->handle_other_msg();
        break;
      case OTHER_TIB:
        static_cast<oti_type*>(this)->handle_other_msg();
        break;
      case OTHER_TIB2:
        static_cast<oti2_type*>(this)->handle_other_msg();
        break;
    }
  }
  void init_portfolio(portfolio_type* port, int feature_fd) {
    this->m_portfolio = port;
    this->m_feature_fd = feature_fd;
  }

  bool is_ready() {
    bool ret = false;
    switch (m_type) {
      case OTHER_BS:
        ret = static_cast<obs_type*>(this)->prepare();
        break;
      case OTHER_BIB:
        ret = static_cast<obi_type*>(this)->prepare();
        break;
      case OTHER_DR:
        ret = static_cast<odr_type*>(this)->prepare();
        break;
      case OTHER_TIB:
        ret = static_cast<oti_type*>(this)->prepare();
        break;
      case OTHER_TIB2:
        ret = static_cast<oti2_type*>(this)->prepare();
        break;

      case SELF_BIB:
        ret = static_cast<sbi_type*>(this)->prepare();
        break;
      case SELF_DR:
        ret = static_cast<sdr_type*>(this)->prepare();
        break;
      case SELF_OFIB:
        ret = static_cast<soi_type*>(this)->prepare();
        break;
      case SELF_TIB:
        ret = static_cast<sti_type*>(this)->prepare();
        break;
      case SELF_TIB2:
        ret = static_cast<sti2_type*>(this)->prepare();
        break;
    }
    return ret;
  }

  BOOST_PARAMETER_MEMBER_FUNCTION(
      (void),                                // return type
      init,                                  // function name
      feature_tag,                           // namespace of tag types
      (required                              // required parameters
       (feature_type, (const int))           // feature type
       (feature_name, (const char*))         // feature name
       (trading_fd, (int))                   // trading descriptor
       (contract_size, (int))                // contract size
       (tick_size, (int))                    // tick size
       )                                     // end of required parameters
      (deduced                               // deduced parameters
       (optional                             // optional parameters
        (book_decay, (double), 1000)         // book decay
        (reference_fd, (int), m_quote_size)  // reference alias
        (ratio_mics, (double), 1000)         // ratio_mics
        (use_diff, (bool), false)            // use_diff
        (corr_negative, (bool), false)       // corr_negative

        )  // end of optional parameters
       )   // end of deduced parameters

  ) {
    m_type = feature_type;

    switch (m_type) {
      case OTHER_BS:
        static_cast<obs_type*>(this)->init(trading_fd, reference_fd, ratio_mics,
                                           use_diff, corr_negative);
        break;
      case OTHER_BIB:
        static_cast<obi_type*>(this)->init(trading_fd, reference_fd, tick_size,
                                           ratio_mics, use_diff, corr_negative);
        break;
      case OTHER_DR:
        static_cast<odr_type*>(this)->init(trading_fd, reference_fd,
                                           book_decay);
        break;
      case OTHER_TIB:
        static_cast<oti_type*>(this)->init(trading_fd, reference_fd, tick_size,
                                           contract_size, ratio_mics, use_diff,
                                           corr_negative);
        break;
      case OTHER_TIB2:
        static_cast<oti2_type*>(this)->init(trading_fd, reference_fd, tick_size,
                                            contract_size, ratio_mics, use_diff,
                                            corr_negative, book_decay);
        break;

      case SELF_BIB:
        static_cast<sbi_type*>(this)->init(trading_fd, tick_size);
        break;
      case SELF_DR:
        static_cast<sdr_type*>(this)->init(trading_fd, book_decay);
        break;
      case SELF_OFIB:
        static_cast<soi_type*>(this)->init(trading_fd);
        break;
      case SELF_TIB:
        static_cast<sti_type*>(this)->init(trading_fd, contract_size,
                                           tick_size);
        break;
      case SELF_TIB2:
        static_cast<sti2_type*>(this)->init(trading_fd, contract_size,
                                            tick_size, book_decay);
        break;
    }
  }
};

#define USING_OPEN_FEATURE_DATA() USING_FEATURE_DATA(fm_open_feature<this_type>)

}  // namespace lmice

#endif  // OPENFEATURE_H
