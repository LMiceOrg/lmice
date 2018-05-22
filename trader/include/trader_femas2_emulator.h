#ifndef INCLUDE_TRADER_FEMAS2_EMULATOR_H_
#define INCLUDE_TRADER_FEMAS2_EMULATOR_H_

#include "eal/lmice_eal_shm.h"

#include "femas2/include/USTPFtdcTraderApi.h"

#include "include/worker_board.h"

#include "include/trader_femas2_data.h"
#include "include/trader_femas2_spi.h"

namespace lmice {

class ft_trader_femas2_emulator : public CUstpFtdcTraderApi {
 private:
  lm_worker_board *m_board;
  ft_trader_femas2_api_data *m_api_data;
  ft_trader_femas2_spi_data *m_spi_data;
  CUstpFtdcTraderSpi *m_spi;
  char m_datetime[16];

  int start_worker(int reqid, int mask);

 public:
  inline void set_board(lm_worker_board *board) { m_board = board; }
  inline void set_data(ft_trader_femas2_api_data *data) { m_api_data = data; }
  inline void set_spi_data(ft_trader_femas2_spi_data *data) {
    m_spi_data = data;
  }
  ft_trader_femas2_emulator();
  virtual ~ft_trader_femas2_emulator();

  ///删除接口对象本身
  ///@remark 不再使用本接口对象时,调用该函数删除接口对象
  virtual void Release();

  ///初始化
  ///@remark 初始化运行环境,只有调用后,接口才开始工作
  virtual void Init();

  ///等待接口线程结束运行
  ///@return 线程退出代码
  virtual int Join();

  ///获取当前交易日
  ///@retrun 获取到的交易日
  ///@remark 只有登录成功后,才能得到正确的交易日
  virtual const char *GetTradingDay();

  ///注册前置机网络地址
  ///@param pszFrontAddress：前置机网络地址。
  ///@remark
  ///网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。
  ///@remark
  ///“tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
  virtual void RegisterFront(char *pszFrontAddress);

  ///注册名字服务器网络地址
  ///@param pszNsAddress：名字服务器网络地址。
  ///@remark
  ///网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:12001”。
  ///@remark
  ///“tcp”代表传输协议，“127.0.0.1”代表服务器地址。”12001”代表服务器端口号。
  ///@remark RegisterFront优先于RegisterNameServer
  virtual void RegisterNameServer(char *pszNsAddress);

  ///注册回调接口
  ///@param pSpi 派生自回调接口类的实例
  virtual void RegisterSpi(CUstpFtdcTraderSpi *pSpi);

  /*不用*/
  ///加载证书
  ///@param pszCertFileName 用户证书文件名
  ///@param pszKeyFileName 用户私钥文件名
  ///@param pszCaFileName 可信任CA证书文件名
  ///@param pszKeyFilePassword 用户私钥文件密码
  ///@return 0 操作成功
  ///@return -1 可信任CA证书载入失败
  ///@return -2 用户证书载入失败
  ///@return -3 用户私钥载入失败
  ///@return -4 用户证书校验失败
  virtual int RegisterCertificateFile(const char *pszCertFileName,
                                      const char *pszKeyFileName,
                                      const char *pszCaFileName,
                                      const char *pszKeyFilePassword);

  ///订阅私有流。
  ///@param nResumeType 私有流重传方式
  ///        USTP_TERT_RESTART:从本交易日开始重传
  ///        USTP_TERT_RESUME:从上次收到的续传
  ///        USTP_TERT_QUICK:只传送登录后私有流的内容
  ///@remark 该方法要在Init方法前调用。若不调用则不会收到私有流的数据。
  virtual void SubscribePrivateTopic(USTP_TE_RESUME_TYPE nResumeType);

  ///订阅公共流。
  ///@param nResumeType 公共流重传方式
  ///        USTP_TERT_RESTART:从本交易日开始重传
  ///        USTP_TERT_RESUME:从上次收到的续传
  ///        USTP_TERT_QUICK:只传送登录后公共流的内容
  ///@remark 该方法要在Init方法前调用。若不调用则不会收到公共流的数据。
  virtual void SubscribePublicTopic(USTP_TE_RESUME_TYPE nResumeType);

  /*不用*/
  ///订阅交易员流。
  ///@param nResumeType 交易员流重传方式
  ///        USTP_TERT_RESTART:从本交易日开始重传
  ///        USTP_TERT_RESUME:从上次收到的续传
  ///        USTP_TERT_QUICK:只传送登录后交易员流的内容
  ///@remark 该方法要在Init方法前调用。若不调用则不会收到交易员流的数据。
  virtual void SubscribeUserTopic(USTP_TE_RESUME_TYPE nResumeType);

  /* 设置 5-10秒 */
  ///设置心跳超时时间。
  ///@param timeout 心跳超时时间(秒)
  virtual void SetHeartbeatTimeout(unsigned int timeout);

  /*日志类不用*/
  ///打开请求日志文件
  ///@param pszReqLogFileName 请求日志文件名
  ///@return 0 操作成功
  ///@return -1 打开日志文件失败
  virtual int OpenRequestLog(const char *pszReqLogFileName);

  ///打开应答日志文件
  ///@param pszRspLogFileName 应答日志文件名
  ///@return 0 操作成功
  ///@return -1 打开日志文件失败
  virtual int OpenResponseLog(const char *pszRspLogFileName);

  ///风控前置系统用户登录请求
  virtual int ReqUserLogin(CUstpFtdcReqUserLoginField *pReqUserLogin,
                           int nRequestID);

  ///用户退出请求
  virtual int ReqUserLogout(CUstpFtdcReqUserLogoutField *pReqUserLogout,
                            int nRequestID);

  ///实时生效，可以该密码
  ///用户密码修改请求
  virtual int ReqUserPasswordUpdate(
      CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);

  ///(实时交易报单)
  ///报单录入请求
  virtual int ReqOrderInsert(CUstpFtdcInputOrderField *pInputOrder,
                             int nRequestID);

  ///(实时撤单)
  ///报单操作
  /// OrderSysID 有值， 按照OrderSysID撤单
  /// OrderSysID 没值 UserOrderActionLocalID
  /// 有值，按照UserOrderActionLocalID撤单 UserOrderLocalID
  /// 用户自定义，用来索引撤单操作
  ///报单操作请求
  virtual int ReqOrderAction(CUstpFtdcOrderActionField *pOrderAction,
                             int nRequestID);

  ///(所有查询在实时交易中不使用)
  ///报单查询请求
  virtual int ReqQryOrder(CUstpFtdcQryOrderField *pQryOrder, int nRequestID);

  ///成交单查询请求
  virtual int ReqQryTrade(CUstpFtdcQryTradeField *pQryTrade, int nRequestID);

  ///可用投资者账户查询请求
  virtual int ReqQryUserInvestor(
      CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID);

  ///交易编码查询请求
  virtual int ReqQryTradingCode(CUstpFtdcQryTradingCodeField *pQryTradingCode,
                                int nRequestID);

  ///投资者资金账户查询请求（）
  virtual int ReqQryInvestorAccount(
      CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID);

  ///合约查询请求
  virtual int ReqQryInstrument(CUstpFtdcQryInstrumentField *pQryInstrument,
                               int nRequestID);

  ///交易所查询请求
  virtual int ReqQryExchange(CUstpFtdcQryExchangeField *pQryExchange,
                             int nRequestID);

  ///投资者持仓查询请求（）
  virtual int ReqQryInvestorPosition(
      CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);

  ///订阅主题请求
  virtual int ReqSubscribeTopic(CUstpFtdcDisseminationField *pDissemination,
                                int nRequestID);

  ///合规参数查询请求
  virtual int ReqQryComplianceParam(
      CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID);

  ///主题查询请求 ,21001
  virtual int ReqQryTopic(CUstpFtdcDisseminationField *pDissemination,
                          int nRequestID);

  ///投资者手续费率查询请求
  virtual int ReqQryInvestorFee(CUstpFtdcQryInvestorFeeField *pQryInvestorFee,
                                int nRequestID);

  ///投资者保证金率查询请求
  virtual int ReqQryInvestorMargin(
      CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID);
};

}  // namespace lmice

#endif  // INCLUDE_TRADER_FEMAS2_EMULATOR_H_
