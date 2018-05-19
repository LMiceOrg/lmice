#include <signal.h>

#include "include/trader_xone_emulator.h"
#include "include/trader_xone_spi.h"
typedef lmice::ft_worker_board<1> board_type;
board_type* g_board;

x1ftdcapi::CX1FtdcTraderApi* g_api = nullptr;

void handle_spi_signal(int sig) {
  using namespace lmice;
  if (sig == SIGUSR1) {
    if (g_board->m_signal == Type_CX1FtdcReqUserLoginField) {
      g_api->ReqUserLogin(&g_board->m_api_data.m_req_user_login);
    }
  }
}

int main(int argc, char* argv[])

{
  using lmice::ft_trader_xone_emulator;
  using lmice::ft_trader_xone_spi;
  using lmice::ft_worker_board;
  using x1ftdcapi::CX1FtdcTraderApi;

  int ret;

  ft_trader_xone_spi* spi = nullptr;
  CX1FtdcTraderApi* api = nullptr;

  char front_address[] = "tcp://10.0.0.1:50001";

  // register signal
  signal(SIGUSR1, handle_spi_signal);

  // create shm board
  lmice_shm_t shm_board;
  board_type::create(&shm_board);
  board_type* board = reinterpret_cast<board_type*>(shm_board.addr);
  g_board = board;

  // create spi
  spi = board->m_spi;
  // printf("size of spi %zu, %p \n", sizeof(ft_trader_xone_spi), spi);

  // create api
  api = CX1FtdcTraderApi::CreateCX1FtdcTraderApi();
  if (ft_trader_xone_emulator* demo_api =
          dynamic_cast<ft_trader_xone_emulator*>(api)) {
    lmice_info_print("demo api %d\n", board->m_status);
    demo_api->set_api_data(&board->m_api_data);
    demo_api->set_spi_data(&board->m_spi_data);
    demo_api->set_board(board);
  }
  g_api = api;

  ret = api->Init(front_address, spi);
  if (ret) {
    lmice_error_print("[Init][ERR]:%d=>%s\n", ret,
                      front_address);  //连接失败
  } else {
    printf("[Init][OK]:%d=>%s\n", ret, front_address);  //连接成功
  }

  usleep(100000);
  CX1FtdcReqUserLogoutField req_user_logout;
  req_user_logout.RequestID = 2;
  api->ReqUserLogout(&req_user_logout);
  // printf("req logout\n");
  // spi->OnRspUserLogout(NULL, NULL);

  usleep(200000);
  api->Release();
  api = nullptr;
  spi = nullptr;
  board_type::destroy(&shm_board);
  return 0;
}
