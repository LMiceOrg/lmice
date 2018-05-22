#include <signal.h>

#include <unistd.h>

#include "include/service_logging.h"
#include "include/trader_xone_emulator.h"
#include "include/trader_xone_spi.h"

typedef lmice::lm_worker_board board_type;

board_type* g_board = nullptr;

int main(int argc, char* argv[])

{
  using lmice::ft_trader_xone_api_data;
  using lmice::ft_trader_xone_emulator;
  using lmice::ft_trader_xone_spi;
  using lmice::ft_trader_xone_spi_data;
  using lmice::lm_worker_board;
  using x1ftdcapi::CX1FtdcTraderApi;

  int ret;

  ft_trader_xone_spi* spi = nullptr;
  CX1FtdcTraderApi* api = nullptr;

  ft_trader_xone_spi_data* spi_data;
  ft_trader_xone_api_data* api_data;

  char front_address[] = "tcp://10.0.0.1:50001";
  char login_account[] = "hehao";
  char login_password[] = "123456";

  // create shm board
  lmice_shm_t shm_board;
  board_type::create(&shm_board);
  board_type* board = reinterpret_cast<board_type*>(shm_board.addr);

  g_board = board;

  printf("assign g_board %p %p\n", (void*)&g_board, (void*)g_board);
  // lmice_info_print("done\n");
  // create spi
  int spi_fd = board->create_bulk(sizeof(ft_trader_xone_spi));
  spi = new (board->get_bulk(spi_fd)) ft_trader_xone_spi;
  printf(" spi bulk %p\n", board->get_bulk(spi_fd));

  // create spi data
  int spidata_fd = board->create_bulk(sizeof(lmice::ft_trader_xone_spi_data));
  spi_data = new (board->get_bulk(spidata_fd)) ft_trader_xone_spi_data;
  memset(spi_data, 0, sizeof(ft_trader_xone_spi_data));
  printf("spi status %d %p\n", spi_data->m_status, (void*)spi_data);

  // create api data
  int apidata_fd = board->create_bulk(sizeof(lmice::ft_trader_xone_api_data));
  api_data = new (board->get_bulk(apidata_fd)) ft_trader_xone_api_data;
  memset(api_data, 0, sizeof(ft_trader_xone_api_data));

  printf("spi fd %d %d %d\t %p %p %p\n", spi_fd, spidata_fd, apidata_fd,
         (void*)spi, (void*)spi_data, (void*)api_data);
  spi->set_spi_data(spi_data, api_data, &(board->m_pid), &(board->m_signal));
  //  spi->OnFrontConnected();
  // printf("api type %llu\n", api_data->m_type);

  // spi = &board->m_spi;
  // printf("size of spi %zu, %p \n", sizeof(ft_trader_xone_spi), spi);

  // create api
  api = CX1FtdcTraderApi::CreateCX1FtdcTraderApi();
  if (ft_trader_xone_emulator* demo_api =
          dynamic_cast<ft_trader_xone_emulator*>(api)) {
    demo_api->set_api_data(api_data);
    demo_api->set_spi_data(spi_data);
    demo_api->set_board(board);
    lmice_info_print("demo api %d spi=%p\n", board->m_status, (void*)spi);
  }

  // api init
  lmice_info_print("before init %d\n", spi_data->m_status);
  ret = api->Init(front_address, spi);
  if (ret) {
    lmice_error_print("[Init][ERR]:%d=>%s\n", ret,
                      front_address);  //连接失败
    goto clearBoard;
  } else {
    lmice_info_print("[Init][OK]:%d=>%s\n", ret, front_address);  //连接成功
  }

  // waiting for spi to be connected or disconnected
  if (spi->wait_connected() != lmice::SPI_CONNECT) {
    lmice_error_print("交易前端连接失败\n");
    goto clearPoint;
  }
  lmice_info_print("spi connected\n");

  // subscribe topic before user login
  api->SubscribePrivateTopic(0, 0);

  // api user login
  CX1FtdcReqUserLoginField req_user_login;
  lm_ssprintf(req_user_login.AccountID, login_account);
  lm_ssprintf(req_user_login.Password, login_password);
  api->ReqUserLogin(&req_user_login);

  // waiting for spi to be login
  if (spi->wait_logined() != lmice::SPI_LOGIN) {
    lmice_error_print("登录失败\n");
    goto clearPoint;
  }
  // usleep(300000);
  spi_data->m_status = lmice::SPI_STANDBY;
  api->ReqUserLogin(&req_user_login);
  // usleep(300000);

  // waiting for spi to be login
  if (spi->wait_logined() != lmice::SPI_LOGIN) {
    lmice_error_print("登录失败\n");
    goto clearPoint;
  }

  // usleep(300000);
  spi_data->m_status = lmice::SPI_STANDBY;
  api->ReqUserLogin(&req_user_login);
  // usleep(300000);

  if (spi->wait_logined() != lmice::SPI_LOGIN) {
    lmice_error_print("登录失败\n");
    goto clearPoint;
  }

  // api user logout
  CX1FtdcReqUserLogoutField req_user_logout;
  lm_ssprintf(req_user_logout.AccountID, login_account);
  api->ReqUserLogout(&req_user_logout);

clearPoint:
  usleep(200000);
  api->Release();
  api = nullptr;
  spi = nullptr;
  usleep(1000);
clearBoard:
  board_type::destroy(&shm_board);
  return 0;
}
