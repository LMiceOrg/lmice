/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include <stdlib.h>

/** service */
#include "include/service_logging.h"
#include "include/worker_board_data.h"

#include "include/trader_femas2_spi.h"
namespace lmice {
bool ft_trader_femas2_spi::is_error_occur(CUstpFtdcRspInfoField *err_info,
                                          int nRequestID,
                                          const char *func_name) {
  if (err_info) {
    if (err_info->ErrorID != 0) {
      lmice_error_print(
          "FemasV2 SPI[%s]交易错误应答:[请求ID]:%d,[错误ID]:%d,[错误信息]:%s\n",
          func_name,
          nRequestID,         // 请求ID
          err_info->ErrorID,  // 错误ID
          err_info->ErrorMsg  // 错误信息
      );
      return true;
    }
  }
  return false;
}

ft_trader_femas2_spi::ft_trader_femas2_spi() {}

ft_trader_femas2_spi::~ft_trader_femas2_spi() {}

///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void ft_trader_femas2_spi::OnFrontConnected() {
  m_spi_data->m_status = SPI_CONNECT;
  lmice_info_print("Femas2SPI[%s]\n", __FUNCTION__);
}

/// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会
/// 自动重新连接，客户端可不做处理。
/// @param nReason 错误原因
///        0x1001 网络读失败
///        0x1002 网络写失败
///        0x2001 接收心跳超时
///        0x2002 发送心跳失败
///        0x2003 收到错误报文
void ft_trader_femas2_spi::OnFrontDisconnected(int nReason) {
  m_spi_data->m_status = SPI_DISCONNECT;
  lmice_critical_print("Femas2SPI[%s][%d]\n", __FUNCTION__, nReason);
}

/*不需要*/
/// 心跳超时警告。当长时间未收到报文时，该方法被调用。
/// @param nTimeLapse 距离上次接收报文的时间
void ft_trader_femas2_spi::OnHeartBeatWarning(int nTimeLapse) {
  (void)nTimeLapse;
}

/*不需要*/
/// 报文回调开始通知。当API收到一个报文后，首先调用本方法，然后是各数据域
/// 的回调，最后是报文回调结束通知。
/// @param nTopicID 主题代码（如私有流、公共流、行情流等）
/// @param nSequenceNo 报文序号
void ft_trader_femas2_spi::OnPackageStart(int nTopicID, int nSequenceNo) {
  (void)nTopicID;
  (void)nSequenceNo;
}

/*不需要*/
/// 报文回调结束通知。当API收到一个报文后，首先调用报文回调开始通知，然后
/// 是各数据域的回调，最后调用本方法。
/// @param nTopicID 主题代码（如私有流、公共流、行情流等）
/// @param nSequenceNo 报文序号
void ft_trader_femas2_spi::OnPackageEnd(int nTopicID, int nSequenceNo) {
  (void)nTopicID;
  (void)nSequenceNo;
}

/* 其他类型错误 */
/// 错误应答
void ft_trader_femas2_spi::OnRspError(CUstpFtdcRspInfoField *pRspInfo,

                                      int nRequestID, bool bIsLast) {
  (void)bIsLast;
  is_error_occur(pRspInfo, nRequestID, __FUNCTION__);
}

///风控前置系统用户登录应答
void ft_trader_femas2_spi::OnRspUserLogin(
    CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
    m_spi_data->m_status = SPI_CONNECT;
  } else {
    m_spi_data->m_status = SPI_LOGIN;
    m_spi_data->m_request_id = atoi(pRspUserLogin->MaxOrderLocalID) + 1;

    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///用户退出应答
void ft_trader_femas2_spi::OnRspUserLogout(
    CUstpFtdcRspUserLogoutField *pRspUserLogout,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)pRspUserLogout;
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
    m_spi_data->m_status = SPI_RELEASE_API;
  } else {
    m_spi_data->m_status = SPI_CONNECT;

    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///用户密码修改应答
void ft_trader_femas2_spi::OnRspUserPasswordUpdate(
    CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)pUserPasswordUpdate;
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

/*飞马 返回 检查pRspInfo 是否包含错误信息 */
///报单录入应答
void ft_trader_femas2_spi::OnRspOrderInsert(
    CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

/*飞马 返回 检查pRspInfo 是否包含错误信息 */
///报单操作应答
void ft_trader_femas2_spi::OnRspOrderAction(
    CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

/*不使用*/
///数据流回退通知
void ft_trader_femas2_spi::OnRtnFlowMessageCancel(
    CUstpFtdcFlowMessageCancelField *pFlowMessageCancel) {
  (void)pFlowMessageCancel;
  lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
}

/* 交易所给的 成交状态变化 */
///成交回报
void ft_trader_femas2_spi::OnRtnTrade(CUstpFtdcTradeField *pTrade) {
  // 买开
  if (pTrade->OffsetFlag == USTP_FTDC_OF_Open &&
      pTrade->Direction == USTP_FTDC_D_Buy) {
    // buy position
    // trade_status.m_pos.m_buy_pos += pTrade->TradeVolume;
    // left cash
    // m_trade_status.m_acc.m_left_cash -= pTrade->TradeVolume *
    // m_trade_status.m_md.m_multiple * pTrade->TradePrice * ( 1 +
    // m_trade_status.m_md.fee_rate );
  }
  //卖开
  if (pTrade->OffsetFlag == USTP_FTDC_OF_Open &&
      pTrade->Direction == USTP_FTDC_D_Sell) {
    // sell position
    // m_trade_status.m_pos.m_sell_pos += pTrade->TradeVolume;
    // left cash
    // m_trade_status.m_acc.m_left_cash += pTrade->TradeVolume *
    // m_trade_status.m_md.m_multiple * pTrade->TradePrice * ( 1 -
    // m_trade_status.m_md.fee_rate );
  }

  //买平今
  if (pTrade->OffsetFlag == USTP_FTDC_OF_CloseToday &&
      pTrade->Direction == USTP_FTDC_D_Buy) {
    // sell position
    // m_trade_status.m_pos.m_sell_pos -= pTrade->TradeVolume;
    // left cash
    // m_trade_status.m_acc.m_left_cash -=pTrade->TradeVolume
    // *m_trade_status.m_md.m_multiple *pTrade->TradePrice *(1 +
    // m_trade_status.m_md.fee_rate);
  }

  //卖平今
  if (pTrade->OffsetFlag == USTP_FTDC_OF_CloseToday &&
      pTrade->Direction == USTP_FTDC_D_Sell) {
    // buy position
    // m_trade_status.m_pos.m_buy_pos -= pTrade->TradeVolume;
    // left cash
    // m_trade_status.m_acc.m_left_cash += pTrade->TradeVolume *
    // m_trade_status.m_md.m_multiple * pTrade->TradePrice * ( 1 -
    // m_trade_status.m_md.fee_rate );
  }

  //买平昨
  if (pTrade->OffsetFlag == USTP_FTDC_OF_CloseYesterday &&
      pTrade->Direction == USTP_FTDC_D_Buy) {
    // sell position
    // m_trade_status.m_pos.m_yd_buy_pos -= pTrade->TradeVolume;
  }

  //卖平昨
  if (pTrade->OffsetFlag == USTP_FTDC_OF_CloseYesterday &&
      pTrade->Direction == USTP_FTDC_D_Sell) {
    // sell position
    // m_trade_status.m_pos.m_yd_sell_pos -= pTrade->TradeVolume;
  }

  lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
}

/* 交易所给的 状态变化 */
///报单回报
void ft_trader_femas2_spi::OnRtnOrder(CUstpFtdcOrderField *pOrder) {
  lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
}

/* 交易所给的 */
///报单录入错误回报
void ft_trader_femas2_spi::OnErrRtnOrderInsert(
    CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo) {
  if (is_error_occur(pRspInfo, 0, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

/* 交易所给的 */
///报单操作错误回报
void ft_trader_femas2_spi::OnErrRtnOrderAction(
    CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo) {
  if (is_error_occur(pRspInfo, 0, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

/* 交易状态变化 USTP_FTDC_IS_NoTrading 非交易 */
///合约交易状态通知
void ft_trader_femas2_spi::OnRtnInstrumentStatus(
    CUstpFtdcInstrumentStatusField *pInstrumentStatus) {
  lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
}

///账户出入金回报
void ft_trader_femas2_spi::OnRtnInvestorAccountDeposit(
    CUstpFtdcInvestorAccountDepositResField *pInvestorAccountDepositRes) {
  lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
}

///报单查询应答
void ft_trader_femas2_spi::OnRspQryOrder(CUstpFtdcOrderField *pOrder,
                                         CUstpFtdcRspInfoField *pRspInfo,
                                         int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, 0, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///成交单查询应答
void ft_trader_femas2_spi::OnRspQryTrade(CUstpFtdcTradeField *pTrade,
                                         CUstpFtdcRspInfoField *pRspInfo,
                                         int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, 0, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///可用投资者账户查询应答
void ft_trader_femas2_spi::OnRspQryUserInvestor(
    CUstpFtdcRspUserInvestorField *pRspUserInvestor,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///交易编码查询应答
void ft_trader_femas2_spi::OnRspQryTradingCode(
    CUstpFtdcRspTradingCodeField *pRspTradingCode,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///投资者资金账户查询应答
void ft_trader_femas2_spi::OnRspQryInvestorAccount(
    CUstpFtdcRspInvestorAccountField *pRspInvestorAccount,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    // account
    // m_trade_status.m_acc.m_count_id = atoi(pRspInvestorAccount->AccountID);
    // m_trade_status.m_acc.m_left_cash = 0;
    // m_trade_status.m_acc.m_valid_val = pRspInvestorAccount->Available;
    // m_trade_status.m_acc.m_total_val = pRspInvestorAccount->Available +
    // pRspInvestorAccount->ShortMargin + pRspInvestorAccount->LongMargin;
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///合约查询应答
void ft_trader_femas2_spi::OnRspQryInstrument(
    CUstpFtdcRspInstrumentField *pRspInstrument,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///交易所查询应答
void ft_trader_femas2_spi::OnRspQryExchange(
    CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///投资者持仓查询应答
void ft_trader_femas2_spi::OnRspQryInvestorPosition(
    CUstpFtdcRspInvestorPositionField *pRspInvestorPosition,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    // InstrumentID
    if (pRspInvestorPosition->Direction == USTP_FTDC_D_Buy) {
      // m_trade_status.m_pos.m_yd_buy_pos = pRspInvestorPosition->YdPosition;
      // m_trade_status.m_pos.m_buy_pos = pRspInvestorPosition->Position;
    }
    if (pRspInvestorPosition->Direction == USTP_FTDC_D_Sell) {
      // m_trade_status.m_pos.m_yd_sell_pos = pRspInvestorPosition->YdPosition;
      // m_trade_status.m_pos.m_sell_pos = pRspInvestorPosition->Position;
    }
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///订阅主题应答
void ft_trader_femas2_spi::OnRspSubscribeTopic(
    CUstpFtdcDisseminationField *pDissemination,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///合规参数查询应答
void ft_trader_femas2_spi::OnRspQryComplianceParam(
    CUstpFtdcRspComplianceParamField *pRspComplianceParam,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    // pRspComplianceParam->DailyMaxOrder;
    // pRspComplianceParam->DailyMaxOrderAction;
    // DailyMaxOrderVolume;
    // DailyMaxOrderActionVolume;
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///主题查询应答
void ft_trader_femas2_spi::OnRspQryTopic(
    CUstpFtdcDisseminationField *pDissemination,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///投资者手续费率查询应答
void ft_trader_femas2_spi::OnRspQryInvestorFee(
    CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    // g_cur_status.m_md.fee_rate = pInvestorFee->OTFeeRate;
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

///投资者保证金率查询应答
void ft_trader_femas2_spi::OnRspQryInvestorMargin(
    CUstpFtdcInvestorMarginField *pInvestorMargin,
    CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  (void)bIsLast;
  if (is_error_occur(pRspInfo, nRequestID, __FUNCTION__)) {
  } else {
    lmice_info_print("Femas2SPI[%s]", __FUNCTION__);
  }
}

}  // namespace lmice
