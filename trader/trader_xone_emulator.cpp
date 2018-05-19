#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include "eal/lmice_eal_thread.h"
#include "eal/lmice_trace.h"

#include "include/trader_xone_emulator.h"
#include "include/trader_xone_spi.h"

namespace x1ftdcapi {
CX1FtdcTraderApi::CX1FtdcTraderApi() {}
CX1FtdcTraderApi::~CX1FtdcTraderApi() {}
CX1FtdcTraderApi *CX1FtdcTraderApi::CreateCX1FtdcTraderApi(void) {
  return lmice::ft_trader_xone_emulator::CreateCX1FtdcTraderApi();
}
}  // namespace x1ftdcapi

namespace lmice {

void ft_trader_xone_emulator::create_worker(int fd) {
  // setpriority(PRIO_PROCESS, getpid(), PRIO_MIN * 2);
  pid_t pid = fork();
  if (pid == 0) {
    pthread_setname(NULL, "xone_spi");
    lmice_info_print("[create_worker] running %d \n", fd);
    /** set worker process priority */
    //      int pri = getpriority(PRIO_PROCESS, getpid());
    //      int ret = setpriority(PRIO_PROCESS, getpid(), 1);

    m_board->m_workers[fd].m_pid = getpid();

    for (;;) {
      /** process work */
      barrier();
      if (m_board->m_status == LM_BOARD_QUIT) {
        lmice_info_print("[create_worker] stop %d\n", fd);
        exit(0);
      }
      proc_work();
      // cpu_relax();
      usleep(1000);
    }
  }  // end-child
  else if (pid < 0) {
    /** worker create failed */
    int e = errno;
    lmice_error_print("create work process failed[%d]\n", e);
  }
  // lmice_info_print("create worker pid=%d\n", pid);
}

void ft_trader_xone_emulator::proc_work(void) {
  // proc work
  //  printf("proc work\n");
  ft_worker_status &worker = m_board->m_workers[0];
  if (worker.m_status == LM_WORKER_START) {
    uint64_t type = m_api_data->m_type;

    lmice_info_print("type %llu\n", type);

    if (type & Type_OnFrontConnected) {
      m_spi->OnFrontConnected();
    }

    if (type & Type_CX1FtdcReqUserLoginField) {
      m_spi_data->m_rsp_error.ErrorID = 0;
      memcpy(m_spi_data->m_rsp_user_login.AccountID,
             m_api_data->m_req_user_login.AccountID,
             sizeof(m_spi_data->m_rsp_user_login.AccountID));
      m_board->m_spi->OnRspUserLogin(&m_spi_data->m_rsp_user_login,
                                     &m_spi_data->m_rsp_error);
    }

    if (type & Type_CX1FtdcReqUserLogoutField) {
      m_spi_data->m_rsp_error.ErrorID = 0;
      m_spi->OnRspUserLogout(&m_spi_data->m_rsp_user_logout_info,
                             &m_spi_data->m_rsp_error);
    }

    m_api_data->m_type ^= type;

    worker.m_status = LM_WORKER_STOP;
  }
}
int ft_trader_xone_emulator::stop_worker(int fd) {
  int worker_pid = m_board->m_workers[fd].m_pid;
  int signal_zero = 0;
  int proc_status;
  int ret = 1;
  proc_status = kill(worker_pid, signal_zero);
  if (proc_status == ESRCH) {
    ret = LM_WORKER_STOPPED;
  }

  return ret;
}
int ft_trader_xone_emulator::start_worker(int fd, int mask) {
  ft_worker_status &worker = m_board->m_workers[fd];

  if (m_api_data->m_type & mask) {
    lmice_error_print("Too fast to start work\n");
    return 1;
  }
  m_api_data->m_type |= mask;
  worker.m_status = LM_WORKER_START;

  return 0;
}

ft_trader_xone_emulator::ft_trader_xone_emulator() {
  pthread_setname(NULL, "xone_api");
  memset(m_version, 0, sizeof(m_version));
  snprintf(m_version, 8, "demo1.0");
}

ft_trader_xone_emulator::~ft_trader_xone_emulator() {
  for (size_t i = 0; i < m_board->m_workers.size(); ++i) {
    m_board->m_workers[i].m_status = LM_BOARD_QUIT;
  }
}

x1ftdcapi::CX1FtdcTraderApi *ft_trader_xone_emulator::CreateCX1FtdcTraderApi(
    void) {
  ft_trader_xone_emulator *p;
  p = new ft_trader_xone_emulator();
  return static_cast<x1ftdcapi::CX1FtdcTraderApi *>(p);
}

void ft_trader_xone_emulator::Release(void) {
  m_board->m_status = LM_BOARD_QUIT;

  // check worker status
  int worker_fd = LM_SPI_WORKER;
  for (int i = 0; i < 10; ++i) {
    usleep(5000);

    int worker_status = stop_worker(worker_fd);
    if (worker_status == LM_WORKER_STOPPED) {
      break;
    }
  }
  lmice_info_print("[Release]api release\n");
}

int ft_trader_xone_emulator::Init(char *addr, x1ftdcapi::CX1FtdcTraderSpi *pSpi,
                                  int output_core, int input_core) {
  // tcp://172.21.200.103:10910
  size_t addr_len = strlen(addr) + 1;
  char *addr_str = (char *)malloc(addr_len);
  memset(addr_str, 0, addr_len);
  memcpy(addr_str, addr, addr_len - 1);

  char *word;
  char sep[] = {':', '\0'};
  char *brk_colon;

  for (word = strtok_r(addr_str, sep, &brk_colon); word;
       word = strtok_r(NULL, sep, &brk_colon)) {
    if (strncmp(word, "tcp", 4) == 0) {
      m_protocol = TRADER_TCP;
    } else if (strncmp(word, "//", 2) == 0) {
      int a1, a2, a3, a4;
      sscanf(word + 2, "%d.%d.%d.%d", &a1, &a2, &a3, &a4);
      m_front_addr = (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;
    } else if (word) {
      m_front_port = atoi(word);
    }
  }

  m_spi = pSpi;
  m_spi_core = output_core;
  m_api_core = input_core;

  if (m_protocol != TRADER_TCP || m_front_addr == 0 || m_front_port == 0 ||
      m_spi == nullptr) {
    lmice_error_print("Internal error\n");
    return 1;
  }

  create_worker(0);
  usleep(10000);
  lmice_info_print("%p worker pid=%d\n", (void *)m_board,
                   m_board->m_workers[0].m_pid);

  start_worker(0, Type_OnFrontConnected);
  return 0;
}

int ft_trader_xone_emulator::Join(void) {
  Release();

  return 0;
}

void ft_trader_xone_emulator::SubscribePrivateTopic(int priflow_req_flag,
                                                    unsigned int pri_no) {}
void ft_trader_xone_emulator::SetMatchInfoAdvanced(bool matchinfo_advanced) {}

int ft_trader_xone_emulator::ReqUserLogin(
    struct CX1FtdcReqUserLoginField *pUserLoginData) {
  memcpy(&m_api_data->m_req_user_login, pUserLoginData,
         sizeof(CX1FtdcReqUserLoginField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcReqUserLoginField);
  return 0;
}

int ft_trader_xone_emulator::ReqUserLogout(
    struct CX1FtdcReqUserLogoutField *pUserLogoutData) {
  memcpy(&m_api_data->m_req_user_logout, pUserLogoutData,
         sizeof(CX1FtdcReqUserLogoutField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcReqUserLogoutField);
  return 0;
}

int ft_trader_xone_emulator::ReqResetPassword(
    struct CX1FtdcReqResetPasswordField *pResetPasswordData) {
  memcpy(&m_api_data->m_req_reset_password, pResetPasswordData,
         sizeof(CX1FtdcReqResetPasswordField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcReqResetPasswordField);
  return 0;
}

int ft_trader_xone_emulator::ReqInsertOrder(
    struct CX1FtdcInsertOrderField *pInsertOrderData) {
  memcpy(&m_api_data->m_insert_order, pInsertOrderData,
         sizeof(CX1FtdcInsertOrderField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcInsertOrderField);
  return 0;
}

int ft_trader_xone_emulator::ReqCancelOrder(
    CX1FtdcCancelOrderField *pCancelOrderData) {
  memcpy(&m_api_data->m_cancel_order, pCancelOrderData,
         sizeof(CX1FtdcCancelOrderField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcCancelOrderField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryPosition(
    CX1FtdcQryPositionField *pPositionData) {
  memcpy(&m_api_data->m_qry_position, pPositionData,
         sizeof(CX1FtdcQryPositionField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryPositionField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryCustomerCapital(
    CX1FtdcQryCapitalField *pCapitalData) {
  memcpy(&m_api_data->m_qry_capital, pCapitalData,
         sizeof(CX1FtdcQryCapitalField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryCapitalField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryExchangeInstrument(
    struct CX1FtdcQryExchangeInstrumentField *pExchangeInstrumentData) {
  memcpy(&m_api_data->m_qry_exchange_instrument, pExchangeInstrumentData,
         sizeof(CX1FtdcQryExchangeInstrumentField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryExchangeInstrumentField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryOrderInfo(
    struct CX1FtdcQryOrderField *pOrderData) {
  memcpy(&m_api_data->m_qry_order, pOrderData, sizeof(CX1FtdcQryOrderField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryOrderField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryMatchInfo(
    struct CX1FtdcQryMatchField *pMatchData) {
  memcpy(&m_api_data->m_qry_match, pMatchData, sizeof(CX1FtdcQryMatchField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryMatchField);
  return 0;
}

int ft_trader_xone_emulator::ReqQrySpecifyInstrument(
    struct CX1FtdcQrySpecificInstrumentField *pInstrument) {
  memcpy(&m_api_data->m_qry_specific_instrument, pInstrument,
         sizeof(CX1FtdcQrySpecificInstrumentField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQrySpecificInstrumentField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryPositionDetail(
    struct CX1FtdcQryPositionDetailField *pPositionDetailData) {
  memcpy(&m_api_data->m_qry_position_detail, pPositionDetailData,
         sizeof(CX1FtdcQryPositionDetailField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryPositionDetailField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryExchangeStatus(
    struct CX1FtdcQryExchangeStatusField *pQryExchangeStatusData) {
  memcpy(&m_api_data->m_qry_exchange_status, pQryExchangeStatusData,
         sizeof(CX1FtdcQryExchangeStatusField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcQryExchangeStatusField);
  return 0;
}

int ft_trader_xone_emulator::ReqQryArbitrageInstrument(
    struct CX1FtdcAbiInstrumentField *pAbtriInstrumentData) {
  memcpy(&m_api_data->m_abi_instrument, pAbtriInstrumentData,
         sizeof(CX1FtdcAbiInstrumentField));
  start_worker(LM_SPI_WORKER, Type_CX1FtdcAbiInstrumentField);
  return 0;
}

///////以下做市商相关
///
int ft_trader_xone_emulator::ReqQuoteInsert(
    struct CX1FtdcQuoteInsertField *pQuoteInsertOrderData) {
  return 0;
}

int ft_trader_xone_emulator::ReqQuoteCancel(
    CX1FtdcCancelOrderField *pQuoteCancelOrderData) {
  return 0;
}

int ft_trader_xone_emulator::ReqCancelAllOrder(
    CX1FtdcCancelAllOrderField *pCancelAllOrderData) {
  return 0;
}

int ft_trader_xone_emulator::ReqForQuote(CX1FtdcForQuoteField *pForQuoteData) {
  return 0;
}

int ft_trader_xone_emulator::ReqQryForQuote(
    CX1FtdcQryForQuoteField *pQryForQuoteData) {
  return 0;
}

int ft_trader_xone_emulator::ReqQryQuoteOrderInfo(
    CX1FtdcQuoteOrderField *pQuoteOrderData) {
  return 0;
}

int ft_trader_xone_emulator::ReqQryQuoteNotice(
    CX1FtdcQryQuoteNoticeField *pQryQuoteNoticeData) {
  return 0;
}

int ft_trader_xone_emulator::ReqQryArbitrageCombineDetail(
    CX1FtdcArbitrageCombineDetailField *pQryArbitrageCombineDetailData) {
  return 0;
}

const char *ft_trader_xone_emulator::GetVersion() { return m_version; }

}  // namespace lmice
