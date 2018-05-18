#ifndef TRADER_XONE_SPI_H
#define TRADER_XONE_SPI_H
#include "xone/include/X1FtdcTraderApi.h"

namespace lmice {
struct ft_trader_xone_spi_data {
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

class ft_trader_xone_spi : public x1ftdcapi::CX1FtdcTraderSpi {
 private:
  ft_trader_xone_spi_data m_spi_data;

 public:
  /**
   * @brief    网络连接正常响应
   * @details
   * 当客户端与交易后台需建立起通信连接时（还未登录前），客户端API会自动检测与前置机之间的连接，
   *           当网络可用，将自动建立连接，并调用该方法通知客户端，
   * 客户端可以在实现该方法时，重新使用资金账号进行登录。
   *           （该方法是在Api和前置机建立连接后被调用，该调用仅仅是说明tcp连接已经建立成功。用户需要自行登录才能进行后续的业务操作。
   *           连接失败则此方法不会被调用。）
   *
   */
  virtual void OnFrontConnected();

  /**
   * @brief 网络连接不正常响应
   * @details
   * 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端需保证连接成功后有相应的登录逻辑处理。
   * @param    nReason 错误原因
   *           <ul>
   *               <li> 0x1001 查询服务网络读失败
   *               <li> 0x1002 查询服务网络写失败
   *               <li> 0x2002 查询服务发送心跳失败
   *               <li> 0x2003 查询服务收到错误报文
   *               <li> 0x3001 报单服务网络读失败
   *               <li> 0x3002 报单服务网络写失败
   *               <li> 0x4001 报单服务接收心跳超时
   *               <li> 0x4002 报单服务发送心跳失败
   *               <li> 0x4003 报单服务收到错误报文
   *           </ul>
   *
   */
  virtual void OnFrontDisconnected(int nReason){};
  /**
   * @brief   登陆请求响应
   * @details
   * 当用户发出登录请求后，前置机返回响应时此方法会被调用，通知用户登录是否成功。当pUserLoginInfoRtn.loginResult为0时表示登录成功，且登录成功时，pErrorInfo为NULL，否则pErrorInfo中将包含错误ID和错误信息。成功时，用户将获取一个会话ID,当SessionID为1的时候，默认为用户第一次登陆.
   * @param pUserLoginInfoRtn 用户登录信息结构地址。
   * @param pErrorInfo 若登录失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspUserLogin(
      struct CX1FtdcRspUserLoginField* pUserLoginInfoRtn,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief   登出请求响应:
   * @details
   * 当用户发出退出请求后，前置机返回响应此方法会被调用，通知用户退出状态。
   * @param pUserLogoutInfoRtn 返回用户退出信息结构地址。
   * @param pErrorInfo 若登出失败，返回错误信息地址，该结构含有错误信息。
   * @warning 当用户登录返回错误码35,用户需release API 或 重启API后重新登录
   */
  virtual void OnRspUserLogout(
      struct CX1FtdcRspUserLogoutInfoField* pUserLogoutInfoRtn,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief   期货委托报单响应
   * @details
   * 当报单发生错误时，pErrorInfo不为NULL，并在其中包含了错误ID及错误信息，和请求报单时的LocalOrderID，用于对应客户程序的报单。报单成功时，是表示X1台系统确认了该笔报单，该报单也同时报到了交易所，但交易所还未确认。
   * @param pOrderRtn 返回用户下单信息结构地址。
   * @param pErrorInfo 若报单失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pOrderRtn,
                                struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief   期货委托撤单响应
   * @details 当用户撤单后，前置返回响应是该方法会被调用。
   * @param pOrderCanceledRtn 返回撤单响应信息结构地址。
   * @param pErrorInfo 若撤单失败，返回错误信息地址，该结构含有错误信息。
   * @warning
   * 撤单发生错误时，需要检查pErrorInfo是否为NULL。且客户端收到该响应时，只表示柜台确认了这笔撤单请求。
   */
  virtual void OnRspCancelOrder(
      struct CX1FtdcRspOperOrderField* pOrderCanceledRtn,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief   错误回报
   * @details API内部或者柜台内部发生异常错误，通过此方法通知用户。
   *           API内部错误：包含API内存数据异常、系统资源申请异常等。
   *           柜台内存错误：包含交易所API内部抛出异常，柜台内部数据异常，系统资源申请异常等。
   * @param pErrorInfo 错误信息的结构地址。
   */
  virtual void OnRtnErrorMsg(struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief   成交回报
   * @details 当委托成功交易后次方法会被调用。
   * @param pRtnMatchData 指向成交回报的结构的指针。
   */
  virtual void OnRtnMatchedInfo(
      struct CX1FtdcRspPriMatchInfoField* pRtnMatchData){};

  /**
   * @brief   委托回报
   * @details 当交易所收到委托请求并返回回报时，该方法会被调用
   * @param pRtnOrderData 指向委托回报地址的指针。
   */
  virtual void OnRtnOrder(struct CX1FtdcRspPriOrderField* pRtnOrderData){};

  /**
   * @brief  撤单回报
   * @details 当撤单成功后该方法会被调用。
   * @param pCancelOrderData
   * 指向撤单回报结构的地址，该结构体包含被撤单合约的相关信息。
   */
  virtual void OnRtnCancelOrder(
      struct CX1FtdcRspPriCancelOrderField* pCancelOrderData){};

  /**
   * @brief  查询当日委托响应
   * @details 当用户发出委托查询后，该方法会被调用。
   * @param pRtnOrderData 指向委托回报结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast    表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQryOrderInfo(struct CX1FtdcRspOrderField* pRtnOrderData,
                                 struct CX1FtdcRspErrorField* pErrorInfo,
                                 bool bIsLast){};

  /**
   * @brief  查询当日成交响应
   * @details 当用户发出成交查询后该方法会被调用。
   * @param pRtnMatchData 指向成交回报结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQryMatchInfo(struct CX1FtdcRspMatchField* pRtnMatchData,
                                 struct CX1FtdcRspErrorField* pErrorInfo,
                                 bool bIsLast){};

  /**
   * @brief  持仓查询响应
   * @details 当用户发出持仓查询指令后，前置返回响应时该方法会被调用。
   * @param pPositionInfoRtn 返回持仓信息结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQryPosition(
      struct CX1FtdcRspPositionField* pPositionInfoRtn,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief  客户资金查询响应
   * @details 当用户发出资金查询指令后，前置返回响应时该方法会被调用。
   * @param pCapitalInfoRtn 返回资金信息结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspCustomerCapital(
      struct CX1FtdcRspCapitalField* pCapitalInfoRtn,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief  交易所合约查询响应
   * @details 当用户发出合约查询指令后，前置返回响应时该方法会被调用。
   * @param pInstrumentData 返回合约信息结构的地址。
   * @param pErrorInfo 错误信息结构，如果查询发生错误，则返回错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQryExchangeInstrument(
      struct CX1FtdcRspExchangeInstrumentField* pInstrumentData,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief  查询指定合约响应
   * @details 当用户发出指定合约查询指令后，前置返回响应时该方法会被调用。
   * @param pInstrument 返回指定合约信息结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQrySpecifyInstrument(
      struct CX1FtdcRspSpecificInstrumentField* pInstrument,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief  查询持仓明细响应
   * @details 当用户发出查询持仓明细后，前置返回响应时该方法会被调用。
   * @param pPositionDetailRtn 返回持仓明细结构的地址。
   * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast 表明是否是最后一条响应信息
   *                   <ul>
   *                       <li> 0 - 否
   *                       <li> 1 - 是
   *                   </ul>
   */
  virtual void OnRspQryPositionDetail(
      struct CX1FtdcRspPositionDetailField* pPositionDetailRtn,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief 交易所状态查询响应
   * @param pRspExchangeStatusData:指向交易所状态查询响应地址的指针。
   * @warning
   * 若收不到开闭市信号，可先咨询期货公司运维同事，是否在报盘的配置中，打开了<PushJYZT
   * Value=”YES”/>的选项
   */
  virtual void OnRspQryExchangeStatus(
      struct CX1FtdcRspExchangeStatusField* pRspExchangeStatusData){};
  /**
   * @brief 交易所状态通知
   * @details 支持交易所状态通知和品种状态通知，在x1柜台可配
   * @param pRtnExchangeStatusData 指向交易所状态通知地址的指针。
   */
  virtual void OnRtnExchangeStatus(
      struct CX1FtdcExchangeStatusRtnField* pRtnExchangeStatusData){};

  /**
   * @brief 做市商报单响应
   * @param pRspQuoteData:指向做市商报单响应地址的指针。
   * @param pErrorInfo:若报价失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspQuoteInsert(struct CX1FtdcQuoteRspField* pRspQuoteData,
                                struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief 做市商报单回报
   * @param pRtnQuoteData:指向做市商报单回报地址的指针。
   */
  virtual void OnRtnQuoteInsert(struct CX1FtdcQuoteRtnField* pRtnQuoteData){};

  /**
   * @brief 做市商撤单响应
   * @param pRspQuoteCanceledData:指向做市商撤单响应地址的指针。
   * @param pErrorInfo:若撤单失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspQuoteCancel(
      struct CX1FtdcQuoteRspField* pRspQuoteCanceledData,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief 做市商撤单回报
   * @param pRtnQuoteCanceledData:指向做市商撤单回报地址的指针。
   */
  virtual void OnRtnQuoteCancel(
      struct CX1FtdcQuoteCanceledRtnField* pRtnQuoteCanceledData){};

  /**
   * @brief 做市商成交回报
   * @param pRtnQuoteMatchedData:指向做市商成交回报地址的指针。
   */
  virtual void OnRtnQuoteMatchedInfo(
      struct CX1FtdcQuoteMatchRtnField* pRtnQuoteMatchedData){};

  /**
   * @brief 批量撤单响应
   * @param pRspStripCancelOrderData:指向批量撤单响应地址的指针。
   * @param pErrorInfo:若批量撤单失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspCancelAllOrder(
      struct CX1FtdcCancelAllOrderRspField* pRspCancelAllOrderData,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief 询价响应
   * @param pRspForQuoteData:询价请求响应结构地址。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   */
  virtual void OnRspForQuote(struct CX1FtdcForQuoteRspField* pRspForQuoteData,
                             struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * @brief 询价回报
   * @param pRtnForQuoteData:询价回报结构地址。
   */
  virtual void OnRtnForQuote(
      struct CX1FtdcForQuoteRtnField* pRtnForQuoteData){};

  /**
   * @brief 询价委托查询响应
   * @param pRtnQryForQuoteData:指向询价委托查询响应地址的指针。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
   */
  virtual void OnRspQryForQuote(
      struct CX1FtdcQryForQuoteRtnField* pRtnQryForQuoteData,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief 查询当日报价委托响应
   * @param pRtnQuoteOrderData:指向报价查询回报结构的地址。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
   */
  virtual void OnRspQryQuoteOrderInfo(
      struct CX1FtdcQuoteOrderRtnField* pRtnQuoteOrderData,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief 询价通知查询响应
   * @param pRtnQryQuoteNoticeData:查询询价通知回报结构地址。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
   */
  virtual void OnRspQryQuoteNotice(
      struct CX1FtdcQryQuoteNoticeRtnField* pRtnQryQuoteNoticeData,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief
   * 套利合约查询响应:当用户发出套利合约查询指令后，前置返回响应时该方法会被调用。
   * @param pAbiInstrumentData:返回套利合约信息结构的地址。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
   */
  virtual void OnRspArbitrageInstrument(
      struct CX1FtdcAbiInstrumentRtnField* pAbiInstrumentData,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};

  /**
   * @brief 做市商询价通知，无需订阅，做市商客户自动收到询价通知
   * @param pForQuoteRspData:指向询价通知回报地址的指针。
   */
  virtual void OnRtnForQuoteRsp(
      struct CX1FtdcQuoteSubscribeRtnField* pForQuoteRspData){};

  virtual void OnNtyExchangeConnectionStatus(
      CX1FtdcExchangeConnectionStatusRtnField* pExchangeConnectionStatusData){};

  virtual void OnRspResetPassword(
      struct CX1FtdcRspResetPasswordField* pResetPasswordData,
      struct CX1FtdcRspErrorField* pErrorInfo){};

  /**
   * 查询组合持仓明细响应:当用户发出查询持仓明细后，前置返回响应时该方法会被调用。
   * @param pPositionDetailRtn:返回持仓明细结构的地址。
   * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
   * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
   */
  virtual void OnRspQryArbitrageCombineDetail(
      struct CX1FtdcArbitrageCombineDetailRtnField* pPositionDetail,
      struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast){};
};

}  // namespace lmice
#endif  // TRADER_XONE_SPI_H
