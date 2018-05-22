#ifndef INCLUDE_TRADER_XONE_EMULATOR_H_
#define INCLUDE_TRADER_XONE_EMULATOR_H_

#include "eal/lmice_eal_shm.h"

#include "xone/include/X1FtdcTraderApi.h"

#include "include/worker_board.h"

#include "include/trader_xone_data.h"
#include "include/trader_xone_spi.h"

extern lmice::lm_worker_board *g_board;

namespace lmice {

class ft_trader_xone_emulator : public x1ftdcapi::CX1FtdcTraderApi {
 private:
  enum { LM_WORKER_STOPPED = 0, LM_SPI_WORKER = 0 };
  typedef lm_worker_board board_type;
  board_type *m_board;

  void create_worker(int);
  void proc_work(void);
  int stop_worker(int);
  int start_worker(int, int);
  int m_protocol;
  int m_front_addr;
  int m_front_port;
  int m_api_core;
  int m_spi_core;
  char m_version[8];

  ft_trader_xone_api_data *m_api_data;
  ft_trader_xone_spi_data *m_spi_data;

  x1ftdcapi::CX1FtdcTraderSpi *m_spi;
  enum { TRADER_TCP = 1, TRADER_UDP, TRADER_MCAST };

 public:
  inline void set_spi_data(ft_trader_xone_spi_data *data) { m_spi_data = data; }
  inline void set_api_data(ft_trader_xone_api_data *data) { m_api_data = data; }
  inline void set_board(board_type *board) {
    m_board = board;
    g_board = board;
    printf("api gboard %p %p\n", (void *)&g_board, (void *)g_board);
  }
  ft_trader_xone_emulator();
  virtual ~ft_trader_xone_emulator();

  static x1ftdcapi::CX1FtdcTraderApi *CreateCX1FtdcTraderApi(void);

  /**
   * @brief 释放API实例
   * @warning
   * 调用CreateCX1FtdcTraderApi成功生成一个API实例,init成功后，用户程序退出时必须调用Release接口，否则可能会coredump
   */
  virtual void Release(void);

  /**
   * @brief 初始化接口
   * @details 和服务器建立socket连接，并启动一个接收线程，
   * 同时该方法注册一个回调函数集
   * @param pszFrontAddr 交易前置网络地址。
   *                     网络地址的格式为:"protocol://ipaddress:port",如"tcp://172.21.200.103:10910"
   *                     其中protocol的值为tcp格式。
   *                     ipaddress表示交易前置的IP,port表示交易前置的端口
   * @param pSpi 指向CX1FtdcTraderSpi子类对象(回调函数集)的指针
   * @param output_core 输出线程绑定的cpu core id. -1表示不绑定
   * @param input_core 输入线程绑定的cpu core id. -1表示不绑定.input_core
   * output_core不支持仅有一个参数为-1的情形
   * @return 初始化结果
   *         <ul>
   *           <li> 0 - 初始化成功
   *           <li> 1 - 初始化失败 此时可能需要检查OnRtnErrorMsg给出的错误信息
   *         </ul>
   *         Init失败原因可能：
   *               填写的addr格式不正确或者addr中的ip地址及端口不正确，如使用交易API连接到了行情前置的端口
   *               网络问题，可telnet连接ip及port，检查是否畅通
   * @code Usage:
   *   x1ftdcapi::CX1FtdcTraderApi* pApi =
   * x1ftdcapi::CX1FtdcTraderApi::CreateCX1FtdcTraderApi(); MyTradeSpi
   * spi(pApi);   //
   * MyTradeSpi是继承CX1FtdcTraderSpi的子类对象，实现了用户响应接口 int iret =
   * pApi->Init("tcp://127.0.0.1:10700", &spi, 2, 4); if (iret != 0) {
   *       printf("init failed! error:%d\n", iret);
   *       return -1;
   *   }
   *
   *   //do something
   *
   *   pApi->Release();
   * @endcode
   */
  virtual int Init(char *pszFrontAddr, x1ftdcapi::CX1FtdcTraderSpi *pSpi,
                   int output_core = -1, int input_core = -1);

  /**
   * @brief 等待接口线程结束运行。
   * @return 线程退出代码。
   *         <ul>
   *           <li> 0  - 线程退出成功
   *           <li> -1 - 线程退出失败
   *         </ul>
   */
  virtual int Join(void);

  /**
   * @brief 订阅私有流
   * @details 在Init 方法调用之后，ReqUserLogin
   * 方法调用之前调用。若不调用该接口，则收不到私有流
   * @param priflow_req_flag 私有流订阅方式
   *         <ul>
   *           <li> 0 - 只传送登录后私有流的内容
   *           <li> 1 - 从本交易日开始重传
   *           <li> 2 - 从上次收到的续传
   *           <li> 3 - 从指定编号开始续传
   *         </ul>
   * @param pri_no 指定私有流编号(从指定编号处订阅私有流)
   * 当priflow_req_flag取值3时，该参数才有效，其他情况，传入0值即可。注意:
   * 用户收到的私有流编号可能不连续
   */
  virtual void SubscribePrivateTopic(int priflow_req_flag, unsigned int pri_no);

  virtual void SetMatchInfoAdvanced(bool matchinfo_advanced);

  /**
   * @brief 用户发出登录请求
   * @param pUserLoginData 指向用户登录请求结构的地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqUserLogin(struct CX1FtdcReqUserLoginField *pUserLoginData);

  /**
   * @brief 用户发出登出请求
   * @param pUserLogoutData 指向用户登录请出结构的地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqUserLogout(struct CX1FtdcReqUserLogoutField *pUserLogoutData);

  /**
   * @brief 用户发出重置密码请求
   * @param pResetPasswordData 指向用户重置密码请求结构的地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqResetPassword(
      struct CX1FtdcReqResetPasswordField *pResetPasswordData);

  /**
   * @brief 期货委托报单请求。
   * @param pInsertOrderData 用户请求报单信息结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqInsertOrder(struct CX1FtdcInsertOrderField *pInsertOrderData);

  /**
   * @brief 期货撤单请求。
   * @details
   * 如果提供柜台委托号(柜台委托号大于-1)，则只使用柜台委托号处理；只有当柜台委托号小于0时，才使用本地委托号进行撤单
   * @param pCancelOrderData 用户请求撤单信息结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqCancelOrder(struct CX1FtdcCancelOrderField *pCancelOrderData);

  /**
   * @brief 持仓查询请求。
   * @details 如果没有提供合约代码，则查询全部持仓信息。
   * @param pPositionData 用户请求持仓信息结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> -1     -  请求发送失败
   *           <li> -2     -  检测异常
   *         </ul>
   */
  virtual int ReqQryPosition(struct CX1FtdcQryPositionField *pPositionData);

  /**
   * @brief 客户资金查询请求。
   * @details 用户需要填充该结构的各个字段
   * @param pCapitalData 请求资金查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQryCustomerCapital(
      struct CX1FtdcQryCapitalField *pCapitalData);

  /**
   * @brief 查询交易所合约列表（非套利）。
   * @details
   * 当ExchangeID为空时，表示查询各交易所的所有合约代码，也可指定查询某交易所的所有合约代码信息，但该接口不能查询到套利合约代码
   * @param pExchangeInstrumentData 交易所合约查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQryExchangeInstrument(
      struct CX1FtdcQryExchangeInstrumentField *pExchangeInstrumentData);

  /**
   * @brief 当日委托查询请求。
   * @param pOrderData 当日委托查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> -1     -  请求发送失败
   *           <li> -2     -  检测异常
   *         </ul>
   */
  virtual int ReqQryOrderInfo(struct CX1FtdcQryOrderField *pOrderData);

  /**
   * @brief 当日成交查询请求。
   * @param pMatchData 当日成交查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQryMatchInfo(struct CX1FtdcQryMatchField *pMatchData);

  /**
   * @brief 指定合约信息查询请求。
   * @details
   * ExchangeID并非必须输入选项，因为目前期货市场上，一个合约代码在各交易所是唯一存在的。
   * @param pInstrument 指定合约查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQrySpecifyInstrument(
      struct CX1FtdcQrySpecificInstrumentField *pInstrument);

  /**
   * @brief 持仓明细查询请求。
   * @param pPositionDetailData:持仓明细查询结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQryPositionDetail(
      struct CX1FtdcQryPositionDetailField *pPositionDetailData);

  /**
   * @brief 交易所状态查询
   * @param pQryExchangeStatusData:交易所状态查询请求结构地址。
   * @return 返回值
   *         <ul>
   *           <li> 0      -  请求发送成功
   *           <li> 非零   -  错误码
   *         </ul>
   */
  virtual int ReqQryExchangeStatus(
      struct CX1FtdcQryExchangeStatusField *pQryExchangeStatusData);

  /**
   * @brief 查询交易所套利合约列表。
   * @param pAbtriInstrumentData:交易所套利合约查询结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQryArbitrageInstrument(
      struct CX1FtdcAbiInstrumentField *pAbtriInstrumentData);

  ///////以下做市商相关
  /**
   * @brief 做市商报单请求
   * @param pQuoteInsertOrderData:做市商报单请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQuoteInsert(
      struct CX1FtdcQuoteInsertField *pQuoteInsertOrderData);

  /**
   * @brief 做市商撤单请求
   * @details 对于双边应价，本接口只支持撤双边，不支持只撤销一边的操作
   * @param pQuoteCancelOrderData:做市商撤单请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQuoteCancel(
      struct CX1FtdcCancelOrderField *pQuoteCancelOrderData);

  /**
   * @brief 批量撤单
   * @details
   * 该指令目前只支持大商所的做市商报价订单，其它交易所暂不支持，普通期货期权订单也不支持
   * @param pCancelAllOrderData:批量撤单请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqCancelAllOrder(
      struct CX1FtdcCancelAllOrderField *pCancelAllOrderData);

  /**
   * @brief 询价请求
   * @param pForQuoteData:询价请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqForQuote(struct CX1FtdcForQuoteField *pForQuoteData);

  /**
   * @brief 询价委托查询请求
   * @param pQryForQuoteData:询价委托查询请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQryForQuote(struct CX1FtdcQryForQuoteField *pQryForQuoteData);

  /**
   * @brief 做市商报价委托查询
   * @param pQuoteOrderData:做市商报价委托查询结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQryQuoteOrderInfo(
      struct CX1FtdcQuoteOrderField *pQuoteOrderData);

  /**
   * @brief 询价通知查询请求
   * @param pQryQuoteNoticeData:查询询价通知请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQryQuoteNotice(
      struct CX1FtdcQryQuoteNoticeField *pQryQuoteNoticeData);
  ///////以上做市商相关

  /**
   * 组合持仓明细查询请求
   * @param pQryArbitrageCombineDetailData:查询组合持仓请求结构地址。
   * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
   */
  virtual int ReqQryArbitrageCombineDetail(
      struct CX1FtdcArbitrageCombineDetailField
          *pQryArbitrageCombineDetailData);
  /**
   * @brief 获取版本号
   * @return 版本号字符串
   */
  virtual const char *GetVersion();
};

}  // namespace lmice

#endif  // INCLUDE_TRADER_XONE_EMULATOR_H_
