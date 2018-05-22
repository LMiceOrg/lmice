#ifndef INCLUDE_TRADER_FEMAS2_DATA_H_
#define INCLUDE_TRADER_FEMAS2_DATA_H_

#include <stdint.h>

#include "femas2/include/USTPFtdcUserApiStruct.h"

namespace lmice {
enum {
  Type_CUstpFtdcReqUserLoginField,
  Type_CUstpFtdcReqUserLogoutField,
  Type_CUstpFtdcUserPasswordUpdateField,
  Type_CUstpFtdcInputOrderField,
  Type_CUstpFtdcOrderActionField,
  Type_CUstpFtdcQryOrderField,
  Type_CUstpFtdcQryTradeField,
  Type_CUstpFtdcQryUserInvestorField,
  Type_CUstpFtdcQryTradingCodeField,

  Type_CUstpFtdcQryInvestorAccountField,
  Type_CUstpFtdcQryInstrumentField,
  Type_CUstpFtdcQryExchangeField,
  Type_CUstpFtdcQryInvestorPositionField,
  Type_CUstpFtdcDisseminationField,

  Type_CUstpFtdcQryComplianceParamField,
  Type_CUstpFtdcQryInvestorFeeField,
  Type_CUstpFtdcQryInvestorMarginField

};

struct ft_trader_femas2_api_data {
  uint64_t m_type;
  CUstpFtdcReqUserLoginField m_req_user_login;
  CUstpFtdcReqUserLogoutField m_req_user_logout;
  CUstpFtdcUserPasswordUpdateField m_user_password_update;
  CUstpFtdcInputOrderField m_input_order;
  CUstpFtdcOrderActionField m_order_action;

  CUstpFtdcQryOrderField m_qry_order;
  CUstpFtdcQryTradeField m_qry_trade;
  CUstpFtdcQryUserInvestorField m_qry_user_investor;
  CUstpFtdcQryTradingCodeField m_qry_trading_code;
  CUstpFtdcQryInvestorAccountField m_qry_investor_account;
  CUstpFtdcQryInstrumentField m_qry_instrument;
  CUstpFtdcQryExchangeField m_qry_exchange;
  CUstpFtdcQryInvestorPositionField m_qry_investor_position;

  CUstpFtdcDisseminationField m_dissemination;
  CUstpFtdcQryComplianceParamField m_qry_compliance_param;

  CUstpFtdcQryInvestorFeeField m_qry_investor_fee;
  CUstpFtdcQryInvestorMarginField m_qry_investor_margin;
};

enum {};

struct ft_trader_femas2_spi_data {
  int m_status;
  int m_request_id;
};

}  // namespace lmice
#endif  // INCLUDE_TRADER_FEMAS2_DATA_H_
