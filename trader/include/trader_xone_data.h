#ifndef TRADER_INCLUDE_TRADER_XONE_DATA_H_
#define TRADER_INCLUDE_TRADER_XONE_DATA_H_

#include <stdint.h>

#include "xone/include/X1FtdcApiDataType.h"
#include "xone/include/X1FtdcApiStruct.h"
namespace lmice {
enum {
  Type_CX1FtdcReqUserLoginField = (1 << 0),
  Type_CX1FtdcReqUserLogoutField = (1 << 1),
  Type_CX1FtdcReqResetPasswordField = (1 << 2),
  Type_CX1FtdcInsertOrderField = (1 << 3),
  Type_CX1FtdcCancelOrderField = (1 << 4),
  Type_CX1FtdcQryPositionField = (1 << 5),
  Type_CX1FtdcQryCapitalField = (1 << 6),
  Type_CX1FtdcQryExchangeInstrumentField = (1 << 7),
  Type_CX1FtdcQryOrderField = (1 << 8),
  Type_CX1FtdcQryMatchField = (1 << 9),
  Type_CX1FtdcQrySpecificInstrumentField = (1 << 10),
  Type_CX1FtdcQryPositionDetailField = (1 << 11),
  Type_CX1FtdcQryExchangeStatusField = (1 << 12),
  Type_CX1FtdcAbiInstrumentField = (1 << 13),
  Type_CX1FtdcQuoteInsertField = (1 << 14),
  Type_CX1FtdcCancelAllOrderField = (1 << 15),
  Type_CX1FtdcForQuoteField = (1 << 16),
  Type_CX1FtdcQryForQuoteField = (1 << 17),
  Type_CX1FtdcQuoteOrderField = (1 << 18),
  Type_CX1FtdcQryQuoteNoticeField = (1 << 19),
  Type_CX1FtdcArbitrageCombineDetailField = (1 << 20),
  Type_OnFrontConnected = (1 << 21)
};
struct ft_trader_xone_api_data {
  uint64_t m_type;
  CX1FtdcReqUserLoginField m_req_user_login;
  CX1FtdcReqUserLogoutField m_req_user_logout;
  CX1FtdcReqResetPasswordField m_req_reset_password;
  CX1FtdcInsertOrderField m_insert_order;
  CX1FtdcCancelOrderField m_cancel_order;
  CX1FtdcQryPositionField m_qry_position;
  CX1FtdcQryCapitalField m_qry_capital;
  CX1FtdcQryExchangeInstrumentField m_qry_exchange_instrument;
  CX1FtdcQryOrderField m_qry_order;
  CX1FtdcQryMatchField m_qry_match;
  CX1FtdcQrySpecificInstrumentField m_qry_specific_instrument;
  CX1FtdcQryPositionDetailField m_qry_position_detail;
  CX1FtdcQryExchangeStatusField m_qry_exchange_status;
  CX1FtdcAbiInstrumentField m_abi_instrument;
  CX1FtdcQuoteInsertField m_quote_insert;
  CX1FtdcCancelAllOrderField m_cancel_all_order;
  CX1FtdcForQuoteField m_for_quote;
  CX1FtdcQryForQuoteField m_qry_for_quote;
  CX1FtdcQuoteOrderField m_quote_order;
  CX1FtdcQryQuoteNoticeField m_qry_quote_notice;
  CX1FtdcArbitrageCombineDetailField m_arbitrage_combine;
};

enum {
  SPI_STANDBY,
  SPI_CONNECTED,
  SPI_LOGIN,
  SPI_LOGIN_FAILED,
  SPI_RELEASE_API
};

struct ft_trader_xone_spi_data {
  int m_status;
  long m_session_id;
  CX1FtdcRspErrorField m_rsp_error;
  CX1FtdcRspUserLoginField m_rsp_user_login;
  CX1FtdcRspUserLogoutInfoField m_rsp_user_logout_info;
  CX1FtdcRspOperOrderField m_rsp_oper_order;
  CX1FtdcRspPriMatchInfoField m_rsp_pri_match_info;
  CX1FtdcRspPriOrderField m_rsp_pri_order;
  CX1FtdcRspPriCancelOrderField m_rsp_pri_cancel_order;
  CX1FtdcRspOrderField m_rsp_order;
  CX1FtdcRspMatchField m_rsp_match;
  CX1FtdcRspPositionField m_rsp_position;
  CX1FtdcRspCapitalField m_rsp_capital;
  CX1FtdcRspExchangeInstrumentField m_rsp_exchange_instrument;
  CX1FtdcRspSpecificInstrumentField m_rsp_specific_instrument;
  CX1FtdcRspPositionDetailField m_rsp_position_detail;
  CX1FtdcRspExchangeStatusField m_rsp_exchange_status;
  CX1FtdcExchangeStatusRtnField m_exchange_status_rtn;
  //// 做市商
  CX1FtdcQuoteRspField m_quote_rsp;
  CX1FtdcQuoteRtnField m_quote_rtn;
  CX1FtdcQuoteCanceledRtnField m_quote_canceled_rtn;
  CX1FtdcQuoteMatchRtnField m_quote_match_rtn;
  CX1FtdcCancelAllOrderRspField m_cancel_all_order_rsp;
  CX1FtdcForQuoteRspField m_for_quote_rsp;
  CX1FtdcForQuoteRtnField m_for_quote_rtn;
  CX1FtdcQryForQuoteRtnField m_qry_for_quote_rtn;
  CX1FtdcQuoteOrderRtnField m_quote_order_rtn;
  CX1FtdcQryQuoteNoticeRtnField m_qry_quote_notice_rtn;
  CX1FtdcAbiInstrumentRtnField m_abi_instrument_rtn;
  CX1FtdcQuoteSubscribeRtnField m_quote_subscribe_rtn;
  CX1FtdcExchangeConnectionStatusRtnField m_exchange_connection_status_rtn;
  CX1FtdcRspResetPasswordField m_rsp_reset_password;
  CX1FtdcArbitrageCombineDetailRtnField m_arbitrage_combine_detail_rtn;
};

}  // namespace lmice

#endif  // TRADER_INCLUDE_TRADER_XONE_DATA_H_
