/** eal */

/** service */
#include "include/service_logging.h"

#include "include/trader_xone_spi.h"

namespace lmice {
bool ft_trader_xone_spi::is_error_occur(CX1FtdcRspErrorField *err_info,
                                        const char *func_name) {
  if (err_info) {
    //错误ID不为0
    if (err_info->ErrorID != 0) {
      lmice_error_print(
          "X1SPI[%s]交易错误应答:[请求ID]:[%ld],[会话标识]:[%ld],["
          "资金账号]:[%s],[错误ID]:[%d],[柜台委托号]:[%ld],"
          "[本地委托号]:[%ld],[错误信息]:[%s],[合约代码]:[%s]\n",
          func_name,
          err_info->RequestID,     //请求ID
          err_info->SessionID,     //会话标识
          err_info->AccountID,     //资金账号
          err_info->ErrorID,       //错误ID
          err_info->X1OrderID,     //柜台委托号
          err_info->LocalOrderID,  //本地委托号
          err_info->ErrorMsg,      //错误信息
          err_info->InstrumentID   //合约代码
      );
      return true;
    }
  }
  return false;
}

ft_trader_xone_spi::~ft_trader_xone_spi() {}

void ft_trader_xone_spi::OnFrontConnected() {
  lmice_info_print("Xone front connected, do ReqUserLogin\n");
  m_spi_data->m_status = SPI_CONNECT;
}

void ft_trader_xone_spi::OnFrontDisconnected(int nReason) {
  m_spi_data->m_status = SPI_DISCONNECT;
  lmice_critical_print("Xone front disconnected[%d]\n", nReason);
}

void ft_trader_xone_spi::OnRspUserLogin(
    CX1FtdcRspUserLoginField *pUserLoginInfoRtn,
    CX1FtdcRspErrorField *pErrorInfo) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
    m_spi_data->m_status = SPI_LOGIN_FAIL;
  } else {
    m_spi_data->m_status = SPI_LOGIN;
    m_spi_data->m_session_id = pUserLoginInfoRtn->SessionID;
    lmice_info_print(
        "[OnRspUserLogin]交易登录响应:[请求ID]:[%ld],[资金帐号ID]:[%s],["
        "登录结果]"
        ":[%d],[初始本地委托号]:[%ld],[会话ID]:[%ld],[错误ID]:[%d],[错误信息]:["
        "%s],[大商所时间]:[%s],[上期所时间]:[%s],[中金所时间]:[%s],[郑商所时间]"
        ":"
        "[%s],[上能所时间]:[%s]\n",
        pUserLoginInfoRtn->RequestID,         //请求ID
        pUserLoginInfoRtn->AccountID,         //资金帐号ID
        pUserLoginInfoRtn->LoginResult,       //登录结果
        pUserLoginInfoRtn->InitLocalOrderID,  //初始本地委托号
        pUserLoginInfoRtn->SessionID,         //会话ID
        pUserLoginInfoRtn->ErrorID,           //错误ID
        pUserLoginInfoRtn->ErrorMsg,          //错误信息
        pUserLoginInfoRtn->DCEtime,           //大商所时间
        pUserLoginInfoRtn->SHFETime,          //上期所时间
        pUserLoginInfoRtn->CFFEXTime,         //中金所时间
        pUserLoginInfoRtn->CZCETime,          //郑商所时间
        pUserLoginInfoRtn->INETime            //上能所时间
    );
  }
}

void ft_trader_xone_spi::OnRspUserLogout(
    CX1FtdcRspUserLogoutInfoField *pUserLogoutInfoRtn,
    CX1FtdcRspErrorField *pErrorInfo) {
  printf("%s\n", "OnRspUserLogout");
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
    m_spi_data->m_status = SPI_RELEASE_API;

  } else {
    m_spi_data->m_status = SPI_STANDBY;
    lmice_info_print(
        "[OnRspUserLogout]交易登出响应:[退出结果]:[%d],[请求ID]:[%ld],["
        "资金账号]:[%s],[错误ID]:[%d],[错误信息]:[%s]\n",
        pUserLogoutInfoRtn->LogoutResult, pUserLogoutInfoRtn->RequestID,
        pUserLogoutInfoRtn->AccountID, pUserLogoutInfoRtn->ErrorID,
        pUserLogoutInfoRtn->ErrorMsg);
  }
}

void ft_trader_xone_spi::OnRspInsertOrder(CX1FtdcRspOperOrderField *pOrderRtn,
                                          CX1FtdcRspErrorField *pErrorInfo) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    lmice_info_print(
        "[OnRspInsertOrder]下单响应:[请求ID]:[%ld],[资金账户ID]:[%s],["
        "本地委托号]:[%ld],[柜台委托号]:[%ld],[委托状态]:[%d],[合约代码]:[%s],["
        "会话ID]:[%ld]\n",
        pOrderRtn->RequestID,     //请求ID
        pOrderRtn->AccountID,     //资金账户ID
        pOrderRtn->LocalOrderID,  //本地委托号
        pOrderRtn->X1OrderID,     //柜台委托号
        pOrderRtn->OrderStatus,   //委托状态
        pOrderRtn->InstrumentID,  //合约代码
        pOrderRtn->SessionID      //会话ID
    );
  }
}

void ft_trader_xone_spi::OnRspCancelOrder(
    CX1FtdcRspOperOrderField *pOrderCanceledRtn,
    CX1FtdcRspErrorField *pErrorInfo) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    lmice_info_print(
        "[OnRspCancelOrder]撤单响应:[请求ID]:[%ld],[资金账户ID]:[%s],["
        "本地委托号]:[%ld],[柜台委托号]:[%ld],[委托状态]:[%d],[合约代码]:[%s],["
        "会话ID]:[%ld]\n",
        pOrderCanceledRtn->RequestID,     //请求ID
        pOrderCanceledRtn->AccountID,     //资金账户ID
        pOrderCanceledRtn->LocalOrderID,  //本地委托号
        pOrderCanceledRtn->X1OrderID,     //柜台委托号
        pOrderCanceledRtn->OrderStatus,   //委托状态
        pOrderCanceledRtn->InstrumentID,  //合约代码
        pOrderCanceledRtn->SessionID      //会话ID
    );
  }
}

void ft_trader_xone_spi::OnRtnErrorMsg(CX1FtdcRspErrorField *pErrorInfo) {
  is_error_occur(pErrorInfo, __FUNCTION__);
}

void ft_trader_xone_spi::OnRtnOrder(
    struct CX1FtdcRspPriOrderField *pRtnOrderData) {
  lmice_info_print(
      "[OnRtnOrder]下单回报:[资金账号]:[%s],[本地委托号]:[%ld],[柜台委托号]:[%"
      "ld],[报单编号]:[%s],[委托状态]:[%d],[会话ID]:[%ld],[挂起时间]:[%s],["
      "合约代码]:[%s],[交易所]:[%s],[买卖]:[%d],[开平]:[%d],[合约类型]:[%d],["
      "投资类别]:[%d],[委托价]:[%lf],[撤单数量]:[%ld],[委托数量]:[%ld],["
      "交易编码]:[%s],[状态信息]:[%s],私有流编号:[%ld]\n",
      pRtnOrderData->AccountID,       //资金账号
      pRtnOrderData->LocalOrderID,    //本地委托号
      pRtnOrderData->X1OrderID,       //柜台委托号
      pRtnOrderData->OrderSysID,      //报单编号
      pRtnOrderData->OrderStatus,     //委托状态
      pRtnOrderData->SessionID,       //会话ID
      pRtnOrderData->SuspendTime,     //挂起时间
      pRtnOrderData->InstrumentID,    //合约代码
      pRtnOrderData->ExchangeID,      //交易所
      pRtnOrderData->BuySellType,     //买卖
      pRtnOrderData->OpenCloseType,   //开平
      pRtnOrderData->InstrumentType,  //合约类型
      pRtnOrderData->Speculator,      //投资类别
      pRtnOrderData->InsertPrice,     //委托价
      pRtnOrderData->CancelAmount,    //撤单数量
      pRtnOrderData->OrderAmount,     //委托数量
      pRtnOrderData->ClientID,        //交易编码
      pRtnOrderData->StatusMsg,       //状态信息
      pRtnOrderData->PriFlowNo        //私有流编号
  );
}

void ft_trader_xone_spi::OnRtnCancelOrder(
    CX1FtdcRspPriCancelOrderField *pCancelOrderData) {
  lmice_info_print(
      "[OnRtnCancelOrder]撤单回报:[资金账号]:[%s],[本地委托号]:[%ld],["
      "柜台委托号]:[%ld],[报单编号]:[%s],[申报结果]:[%d],[会话标识]:[%ld],["
      "合约代码]:[%s],[报单价格]:[%lf],[买卖类型]:[%d],[开平标志]:[%d],["
      "撤单数量]:[%ld],[投保]:[%d],[交易所ID]:[%s],[撤单时间]:[%s],[合约类型]:["
      "%d],[委托数量]:[%ld],[保证金]:[%lf],[手续费]:[%lf],[自定义类别]:[%s],["
      "止盈止损价格]:[%lf],[最小成交量]:[%ld],[自动单类别]:[%d],[交易编码]:[%s]"
      ",[状态信息]:[%s],[报单附加属性]:[%c]\n",
      pCancelOrderData->AccountID,        //资金账号
      pCancelOrderData->LocalOrderID,     //本地委托号
      pCancelOrderData->X1OrderID,        //柜台委托号
      pCancelOrderData->OrderSysID,       //报单编号
      pCancelOrderData->OrderStatus,      //申报结果
      pCancelOrderData->SessionID,        //会话标识
      pCancelOrderData->InstrumentID,     //合约代码
      pCancelOrderData->InsertPrice,      //报单价格
      pCancelOrderData->BuySellType,      //买卖类型
      pCancelOrderData->OpenCloseType,    //开平标志
      pCancelOrderData->CancelAmount,     //撤单数量
      pCancelOrderData->Speculator,       //投保
      pCancelOrderData->ExchangeID,       //交易所ID
      pCancelOrderData->CanceledTime,     //撤单时间
      pCancelOrderData->InstrumentType,   //合约类型
      pCancelOrderData->OrderAmount,      //委托数量
      pCancelOrderData->Margin,           //保证金
      pCancelOrderData->Fee,              //手续费
      pCancelOrderData->CustomCategory,   //自定义类别
      pCancelOrderData->ProfitLossPrice,  //止盈止损价格
      pCancelOrderData->MinMatchAmount,   //最小成交量
      pCancelOrderData->InsertType,       //自动单类别
      pCancelOrderData->ClientID,         //交易编码
      pCancelOrderData->StatusMsg,        //错误信息
      pCancelOrderData->OrderProperty     //报单附加属性
  );
}

void ft_trader_xone_spi::OnRspQryOrderInfo(CX1FtdcRspOrderField *pRtnOrderData,
                                           CX1FtdcRspErrorField *pErrorInfo,
                                           bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQryOrderInfo]\n");
  }
}

void ft_trader_xone_spi::OnRspQryMatchInfo(CX1FtdcRspMatchField *pRtnMatchData,
                                           CX1FtdcRspErrorField *pErrorInfo,
                                           bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQryMatchInfo]\n");
  }
}

void ft_trader_xone_spi::OnRspQryPosition(
    CX1FtdcRspPositionField *pPositionInfoRtn, CX1FtdcRspErrorField *pErrorInfo,
    bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQryPosition]\n");
  }
}

void ft_trader_xone_spi::OnRspCustomerCapital(
    CX1FtdcRspCapitalField *pCapitalInfoRtn, CX1FtdcRspErrorField *pErrorInfo,
    bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspCustomerCapital]\n");
  }
}

void ft_trader_xone_spi::OnRspQryExchangeInstrument(
    CX1FtdcRspExchangeInstrumentField *pInstrumentData,
    CX1FtdcRspErrorField *pErrorInfo, bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQryExchangeInstrument]\n");
  }
}

void ft_trader_xone_spi::OnRspQrySpecifyInstrument(
    CX1FtdcRspSpecificInstrumentField *pInstrument,
    CX1FtdcRspErrorField *pErrorInfo, bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQrySpecifyInstrument]\n");
  }
}

void ft_trader_xone_spi::OnRspQryPositionDetail(
    CX1FtdcRspPositionDetailField *pPositionDetailRtn,
    CX1FtdcRspErrorField *pErrorInfo, bool bIsLast) {
  if (is_error_occur(pErrorInfo, __FUNCTION__)) {
  } else {
    // process order info
    lmice_info_print("[OnRspQryPositionDetail]\n");
  }
}

void ft_trader_xone_spi::OnRspQryExchangeStatus(
    CX1FtdcRspExchangeStatusField *pRspExchangeStatusData) {
  lmice_info_print("[OnRspQryExchangeStatus]\n");
}

void ft_trader_xone_spi::OnRtnExchangeStatus(
    CX1FtdcExchangeStatusRtnField *pRtnExchangeStatusData) {
  lmice_info_print("[OnRtnExchangeStatus]\n");
}

}  // namespace lmice
