#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

/** eal */
#include "eal/lmice_eal_thread.h"

/** service */
#include "include/service_logging.h"
#include "include/worker_board_data.h"

/** futuremodel */
#include "include/fastmath.h"

/** trader */
#include "include/trader_femas2_emulator.h"

extern lmice::lm_worker_board *g_board;

///创建TraderApi
///@param pszFlowPath 存贮订阅信息文件的目录，默认为当前目录
///@return 创建出的UserApi
CUstpFtdcTraderApi *CUstpFtdcTraderApi::CreateFtdcTraderApi(
    const char *pszFlowPath) {
  lmice::ft_trader_femas2_emulator *api;
  (void)pszFlowPath;
  api = new lmice::ft_trader_femas2_emulator();
  return reinterpret_cast<CUstpFtdcTraderApi *>(api);
}

///获取系统版本号
///@param nMajorVersion 主版本号
///@param nMinorVersion 子版本号
///@return 系统标识字符串
const char *CUstpFtdcTraderApi::GetVersion(int &nMajorVersion,
                                           int &nMinorVersion) {
  static char s_version[] = "femasv2 demo api 1.0";
  nMajorVersion = 2;
  nMinorVersion = 0;
  return s_version;
}

namespace lmice {

int ft_trader_femas2_emulator::start_worker(int reqid, int mask) {
  lm_worker_status &worker = m_board->m_workers[LM_SPI_WORKER];
  if (m_api_data->m_type & mask) {
    lmice_error_print("Too fast to start work\n");
    return 1;
  }
  m_api_data->m_type |= mask;
  worker.m_status = LM_WORKER_START;

  return 0;
}

ft_trader_femas2_emulator::ft_trader_femas2_emulator() {}

ft_trader_femas2_emulator::~ft_trader_femas2_emulator() {}

///删除接口对象本身
///@remark 不再使用本接口对象时,调用该函数删除接口对象
void ft_trader_femas2_emulator::Release() {
  m_board->m_status = LM_BOARD_QUIT;

  lmice_info_print("[Release]api release\n");
}

///初始化
///@remark 初始化运行环境,只有调用后,接口才开始工作
void ft_trader_femas2_emulator::Init() {}

///等待接口线程结束运行
///@return 线程退出代码
int ft_trader_femas2_emulator::Join() { return 0; }

///获取当前交易日
///@retrun 获取到的交易日
///@remark 只有登录成功后,才能得到正确的交易日
const char *ft_trader_femas2_emulator::GetTradingDay() {
  fast_datetime(m_datetime, 16);
  return m_datetime;
}

///注册前置机网络地址
///@param pszFrontAddress：前置机网络地址。
///@remark
///网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。
///@remark
///“tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
void ft_trader_femas2_emulator::RegisterFront(char *pszFrontAddress) {
  (void)pszFrontAddress;
}

///注册名字服务器网络地址
///@param pszNsAddress：名字服务器网络地址。
///@remark
///网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:12001”。
///@remark
///“tcp”代表传输协议，“127.0.0.1”代表服务器地址。”12001”代表服务器端口号。
///@remark RegisterFront优先于RegisterNameServer
void ft_trader_femas2_emulator::RegisterNameServer(char *pszNsAddress) {
  (void)pszNsAddress;
}

///注册回调接口
///@param pSpi 派生自回调接口类的实例
void ft_trader_femas2_emulator::RegisterSpi(CUstpFtdcTraderSpi *pSpi) {
  m_spi = pSpi;
}

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
int ft_trader_femas2_emulator::RegisterCertificateFile(
    const char *pszCertFileName, const char *pszKeyFileName,
    const char *pszCaFileName, const char *pszKeyFilePassword) {
  (void)pszCertFileName;
  (void)pszKeyFileName;
  (void)pszCaFileName;
  (void)pszKeyFilePassword;
  return 0;
}

///订阅私有流。
///@param nResumeType 私有流重传方式
///        USTP_TERT_RESTART:从本交易日开始重传
///        USTP_TERT_RESUME:从上次收到的续传
///        USTP_TERT_QUICK:只传送登录后私有流的内容
///@remark 该方法要在Init方法前调用。若不调用则不会收到私有流的数据。
void ft_trader_femas2_emulator::SubscribePrivateTopic(
    USTP_TE_RESUME_TYPE nResumeType) {
  (void)nResumeType;
}

///订阅公共流。
///@param nResumeType 公共流重传方式
///        USTP_TERT_RESTART:从本交易日开始重传
///        USTP_TERT_RESUME:从上次收到的续传
///        USTP_TERT_QUICK:只传送登录后公共流的内容
///@remark 该方法要在Init方法前调用。若不调用则不会收到公共流的数据。
void ft_trader_femas2_emulator::SubscribePublicTopic(
    USTP_TE_RESUME_TYPE nResumeType) {
  (void)nResumeType;
}

/*不用*/
///订阅交易员流。
///@param nResumeType 交易员流重传方式
///        USTP_TERT_RESTART:从本交易日开始重传
///        USTP_TERT_RESUME:从上次收到的续传
///        USTP_TERT_QUICK:只传送登录后交易员流的内容
///@remark 该方法要在Init方法前调用。若不调用则不会收到交易员流的数据。
void ft_trader_femas2_emulator::SubscribeUserTopic(
    USTP_TE_RESUME_TYPE nResumeType) {
  (void)nResumeType;
}

/* 设置 5-10秒 */
///设置心跳超时时间。
///@param timeout 心跳超时时间(秒)
void ft_trader_femas2_emulator::SetHeartbeatTimeout(unsigned int timeout) {
  (void)timeout;
}

/*日志类不用*/
///打开请求日志文件
///@param pszReqLogFileName 请求日志文件名
///@return 0 操作成功
///@return -1 打开日志文件失败
int ft_trader_femas2_emulator::OpenRequestLog(const char *pszReqLogFileName) {
  (void)pszReqLogFileName;
  return 0;
}

///打开应答日志文件
///@param pszRspLogFileName 应答日志文件名
///@return 0 操作成功
///@return -1 打开日志文件失败
int ft_trader_femas2_emulator::OpenResponseLog(const char *pszRspLogFileName) {
  (void)pszRspLogFileName;
  return 0;
}

///风控前置系统用户登录请求
int ft_trader_femas2_emulator::ReqUserLogin(
    CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID) {
  auto &req = m_api_data->m_req_user_login;
  // prepare data
  memcpy(&req, pReqUserLogin, sizeof(req));

  // start spi worker
  start_worker(nRequestID, Type_CUstpFtdcReqUserLoginField);
  return 0;
}

///用户退出请求
int ft_trader_femas2_emulator::ReqUserLogout(
    CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID) {
  auto &req = m_api_data->m_req_user_logout;
  // prepare data
  memcpy(&req, pReqUserLogout, sizeof(req));

  start_worker(nRequestID, Type_CUstpFtdcReqUserLogoutField);
  return 0;
}

///实时生效，可以该密码
///用户密码修改请求
int ft_trader_femas2_emulator::ReqUserPasswordUpdate(
    CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) {
  auto &req = m_api_data->m_user_password_update;
  memcpy(&req, pUserPasswordUpdate, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcUserPasswordUpdateField);
  return 0;
}

///(实时交易报单)
///报单录入请求
int ft_trader_femas2_emulator::ReqOrderInsert(
    CUstpFtdcInputOrderField *pInputOrder, int nRequestID) {
  auto &req = m_api_data->m_input_order;
  memcpy(&req, pInputOrder, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcInputOrderField);
  return 0;
}

///(实时撤单)
///报单操作
/// OrderSysID 有值， 按照OrderSysID撤单
/// OrderSysID 没值 UserOrderActionLocalID
/// 有值，按照UserOrderActionLocalID撤单 UserOrderLocalID
/// 用户自定义，用来索引撤单操作
///报单操作请求
int ft_trader_femas2_emulator::ReqOrderAction(
    CUstpFtdcOrderActionField *pOrderAction, int nRequestID) {
  auto &req = m_api_data->m_order_action;
  memcpy(&req, pOrderAction, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcOrderActionField);
  return 0;
}

///(所有查询在实时交易中不使用)
///报单查询请求
int ft_trader_femas2_emulator::ReqQryOrder(CUstpFtdcQryOrderField *pQryOrder,
                                           int nRequestID) {
  auto &req = m_api_data->m_qry_order;
  memcpy(&req, pQryOrder, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryOrderField);
  return 0;
}

///成交单查询请求
int ft_trader_femas2_emulator::ReqQryTrade(CUstpFtdcQryTradeField *pQryTrade,
                                           int nRequestID) {
  auto &req = m_api_data->m_qry_trade;
  memcpy(&req, pQryTrade, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryTradeField);
  return 0;
}

///可用投资者账户查询请求
int ft_trader_femas2_emulator::ReqQryUserInvestor(
    CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID) {
  auto &req = m_api_data->m_qry_user_investor;
  memcpy(&req, pQryUserInvestor, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryUserInvestorField);
  return 0;
}

///交易编码查询请求
int ft_trader_femas2_emulator::ReqQryTradingCode(
    CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID) {
  auto &req = m_api_data->m_qry_trading_code;
  memcpy(&req, pQryTradingCode, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryTradingCodeField);
  return 0;
}

///投资者资金账户查询请求（）
int ft_trader_femas2_emulator::ReqQryInvestorAccount(
    CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID) {
  auto &req = m_api_data->m_qry_investor_account;
  memcpy(&req, pQryInvestorAccount, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryInvestorAccountField);
  return 0;
}

///合约查询请求
int ft_trader_femas2_emulator::ReqQryInstrument(
    CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID) {
  auto &req = m_api_data->m_qry_instrument;
  memcpy(&req, pQryInstrument, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryInstrumentField);
  return 0;
}

///交易所查询请求
int ft_trader_femas2_emulator::ReqQryExchange(
    CUstpFtdcQryExchangeField *pQryExchange, int nRequestID) {
  auto &req = m_api_data->m_qry_exchange;
  memcpy(&req, pQryExchange, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryExchangeField);
  return 0;
}

///投资者持仓查询请求（）
int ft_trader_femas2_emulator::ReqQryInvestorPosition(
    CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID) {
  auto &req = m_api_data->m_qry_investor_position;
  memcpy(&req, pQryInvestorPosition, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryInvestorPositionField);
  return 0;
}

///订阅主题请求
int ft_trader_femas2_emulator::ReqSubscribeTopic(
    CUstpFtdcDisseminationField *pDissemination, int nRequestID) {
  auto &req = m_api_data->m_dissemination;
  memcpy(&req, pDissemination, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcDisseminationField);
  return 0;
}

///合规参数查询请求
int ft_trader_femas2_emulator::ReqQryComplianceParam(
    CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID) {
  auto &req = m_api_data->m_qry_compliance_param;
  memcpy(&req, pQryComplianceParam, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryComplianceParamField);
  return 0;
}

///主题查询请求 ,21001
int ft_trader_femas2_emulator::ReqQryTopic(
    CUstpFtdcDisseminationField *pDissemination, int nRequestID) {
  auto &req = m_api_data->m_dissemination;
  memcpy(&req, pDissemination, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcDisseminationField);
  return 0;
}

///投资者手续费率查询请求
int ft_trader_femas2_emulator::ReqQryInvestorFee(
    CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID) {
  auto &req = m_api_data->m_qry_investor_fee;
  memcpy(&req, pQryInvestorFee, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryInvestorFeeField);
  return 0;
}

///投资者保证金率查询请求
int ft_trader_femas2_emulator::ReqQryInvestorMargin(
    CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID) {
  auto &req = m_api_data->m_qry_investor_margin;
  memcpy(&req, pQryInvestorMargin, sizeof(req));
  start_worker(nRequestID, Type_CUstpFtdcQryInvestorMarginField);
  return 0;
}

}  // namespace lmice
