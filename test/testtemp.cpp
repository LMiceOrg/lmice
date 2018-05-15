
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include <type_traits>
#include <array>

#include "../futuremodel/include/fastmath.h"
namespace lmice {
template <class sub_class>
struct fm_type_traits;

template <template <typename> class sub_class, typename ft_type>
struct fm_type_traits<sub_class<ft_type>> {
  typedef ft_type float_type;
};

template <template <typename, int, int> class sub_class, typename ft_type,
          int quote_size, int quote_depth>
struct fm_type_traits<sub_class<ft_type, quote_size, quote_depth>> {
  typedef ft_type float_type;
  enum { m_quote_size = quote_size, m_quote_depth = quote_depth };
};

template <template <typename, int, int, int> class sub_class, typename ft_type,
          int quote_size, int quote_depth, int contract_size>
struct fm_type_traits<
    sub_class<ft_type, quote_size, quote_depth, contract_size>> {
  typedef ft_type float_type;
  enum {
    m_quote_size = quote_size,
    m_quote_depth = quote_depth,
    m_contract_size = contract_size
  };
};

template <template <typename, int, int, int, int> class sub_class,
          typename ft_type, int quote_size, int quote_depth, int contract_size,
          int feature_size>
struct fm_type_traits<
    sub_class<ft_type, quote_size, quote_depth, contract_size, feature_size>> {
  typedef ft_type float_type;
  enum {
    m_quote_size = quote_size,
    m_quote_depth = quote_depth,
    m_contract_size = contract_size,
    m_feature_size = feature_size
  };
};

/** product id */
typedef int quote_descriptor;
typedef quote_descriptor quote_d;

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
};

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

/** china l1 msg*/

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
  //  product_id m_inst;
  int64_t m_time_micro;  // time in epoch micro seconds
  float_type m_bid;
  float_type m_offer;
  int32_t m_bid_quantity;
  int32_t m_offer_quantity;
  int32_t m_volume;
  float_type m_notional;
  //  float_type m_limit_up;
  //  float_type m_limit_down;
} ;

template <class float_type>
struct china_l1msg
    : public china_l1msg_base<float_type>,
      public china_l1msg_handler<china_l1msg<float_type>, float_type> {};

/** fm chinal1 msg */

template <class sub_class>
struct fm_chinal1_msg_handler {
  int get_quote_fd(const char* oid) const {
    int i = 0;
    const sub_class* sub = static_cast<const sub_class*>(this);
    product_id tid;
    prod_string(&tid, oid);
    for (i = 0; i < sub->m_quote_size; ++i) {
      if (sub->m_aliases[i] == tid) break;
    }
    return i;
  }
};

template <class sub_class>
struct fm_chinal1_msg_data {
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth,
    m_quote_count = m_quote_size * m_quote_depth
  };
  typedef typename fm_type_traits<sub_class>::float_type float_type;
  typedef std::array<china_l1msg<float_type>, m_quote_depth> quote_msg_type;
  static_assert((m_quote_count % 4) == 0,
                "fm_chinal1_msg quote_size x quote_depth must 4x");

  std::array<quote_msg_type, m_quote_size> m_msgs forcepack(64);
  // std::array<china_l1msg<float_type>, m_quote_count> m_msgs;

  //  std::array<int64_t, m_quote_count> m_time_micro;  // epoch micro seconds
  //  std::array<float_type, m_quote_count> m_bid;
  //  std::array<float_type, m_quote_count> m_offer;
  //  std::array<int32_t, m_quote_count> m_bid_quantity;
  //  std::array<int32_t, m_quote_count> m_offer_quantity;
  //  std::array<int32_t, m_quote_count> m_volume;
  //  std::array<float_type, m_quote_count> m_notional;
  // product_id
  std::array<product_id, m_quote_size> m_inst;
  // alias
  std::array<product_id, m_quote_size> m_aliases;
  //  limit_up
  std::array<float_type, m_quote_size> m_limit_up;
  //  limit_down
  std::array<float_type, m_quote_size> m_limit_down;

} forcepack(128);

template <class float_type, int quote_size, int quote_depth>
struct fm_chinal1_msg
    : public fm_chinal1_msg_data<
          fm_chinal1_msg<float_type, quote_size, quote_depth>>,
      public fm_chinal1_msg_handler<
          fm_chinal1_msg<float_type, quote_size, quote_depth>> {};


template <class sub_class>
struct fm_feature_data {
    typedef typename fm_type_traits<sub_class>::float_type float_type;
  enum {
    m_quote_size = fm_type_traits<sub_class>::m_quote_size,
    m_quote_depth = fm_type_traits<sub_class>::m_quote_depth
  };
  typedef fm_chinal1_msg<float_type, m_quote_size, m_quote_depth> quotes_type;
  typedef fast_ema<float_type> fast_ema;
  // uint64_t m_feature_id;  // feature_base
  // contract
  // product_id m_trading_instrument;    // self_feature
  // product_id m_reference_instrument;  // other feature
  bool m_use_diff;
  bool m_corr_negative;
  int16_t m_type;  // feature type

  int m_self_fd;
  int m_other_fd;
  float_type m_signal;

  float_type m_contract_size;
  float_type m_tick_size;

  fast_ema m_book;
  fast_ema m_ema;



  const quotes_type* m_quotes;
};

template<class float_type, int quote_size, int quote_depth>
struct other_best_since;
template<class float_type, int quote_size, int quote_depth>
struct other_book_imbalance;

template<class float_type, int quote_size, int quote_depth>
struct variant_feature:public
        fm_feature_data<variant_feature<float_type, quote_size, quote_depth>>

{
    typedef fm_feature_data<variant_feature<float_type, quote_size, quote_depth>> data_type;
    using data_type::m_type;  // feature type

    using data_type:: m_self_fd;
    using data_type:: m_other_fd;
    using data_type:: m_signal;

    using data_type:: m_contract_size;
    using data_type:: m_tick_size;

    using data_type:: m_book;
    using data_type:: m_ema;

    using data_type:: m_use_diff;
    using data_type:: m_corr_negative;

    using data_type:: m_quotes;
    typedef other_best_since<float_type, quote_size,quote_depth> obs_type;
    typedef other_book_imbalance<float_type, quote_size,quote_depth> obi_type;



    void init(int fd) {
        if(m_type==1)
            static_cast<obs_type*>(this)->init(fd);
        else if(this->m_type==2)
            static_cast<obi_type*>(this)->init(fd);
    }
    void call() {
        if(m_type==1)
            static_cast<obs_type*>(this)->call();
        else if(this->m_type==2)
            static_cast<obi_type*>(this)->call();
    }
};
template<class sub_class>
struct test_temp {
    //using sub_class::m_self_fd;

    void test()
    {
        int self_fd = static_cast<sub_class*>(this)->m_self_fd;
        printf("test temp test %d\n", self_fd);
    }
};

template<class float_type, int quote_size, int quote_depth>
struct other_best_since
        :public variant_feature<float_type, quote_size,quote_depth>,
        public test_temp<other_best_since<float_type, quote_size,quote_depth> >
{
    typedef variant_feature<float_type, quote_size,quote_depth> data_type;
using data_type:: m_self_fd;
    void init(int fd)
    {
        m_self_fd = fd;
    }

    void call()
    {
        printf("other best since fd=%d\n", m_self_fd);
        this->test();
    }
};

template<class float_type, int quote_size, int quote_depth>
struct other_book_imbalance
        :public variant_feature<float_type, quote_size,quote_depth>
{
    typedef variant_feature<float_type, quote_size,quote_depth> data_type;
using data_type:: m_self_fd;
    void init(int fd)
    {
        m_self_fd = fd;
    }
    void call()
    {
        printf("other_book_imbalance fd=%d\n", this->m_self_fd);
    }
};


//  template<class float_type, int quote_size,int quote_depth>
//  template<typename t=std::integral_constant<int, 1>::type>
//  other_best_since<float_type, quote_size,quote_depth>* variant_feature<float_type, quote_size,quote_depth>::cast()
//  {
//      return static_cast<other_best_since<float_type, quote_size,quote_depth>*>(this);
//       // stop tearing your hair out
//  }


} //namespace lmice
int main()
{using namespace lmice;
    typedef variant_feature<double,6,2> vf_type;
    fm_feature_data<vf_type> data;
    std::integral_constant<int, 1> one_t;
    lmice::variant_feature<double, 6,2> vf;
    vf.m_type = 1;
    vf.init(12);
    vf.call();
    printf("hello world sizeof(%lu, %lu)\n", sizeof(vf), sizeof(data));

    std::cout<<sizeof(bool)<<std::endl;
    return 0;
}

