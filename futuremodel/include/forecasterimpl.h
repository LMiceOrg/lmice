#ifndef FORECASTERIMPL_H
#define FORECASTERIMPL_H
#include <string>
#include <vector>

#include "include/chinal1msg_dep.h"

struct cu1_0_forecaster {
  typedef double float_type;
  std::string get_trading_instrument() const;
  std::vector<std::string> get_subsignal_name() const;

  std::vector<std::string> get_subscriptions() const;

  int pre_proc_msg(const ChinaL1Msg& msg_dep);
  void update(const ChinaL1Msg& msg_dep);
  float_type get_forecast();
  float_type get_forecast1();
  void get_all_signal(const float** sigs, int* size) const;
  void get_all_weight(const float** sigs, int* size) const;
  cu1_0_forecaster();
  ~cu1_0_forecaster();
  void init(struct tm date);

  void* m_pimpl;
  void* m_buffer;
};
#endif  // FORECASTERIMPL_H
