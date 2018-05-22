/** service */
#include "include/service_logging.h"

#include "include/trader_femas2_emulator.h"
#include "include/trader_femas2_spi.h"

typedef lmice::lm_worker_board board_type;

board_type* g_board = nullptr;

int main(int argc, char* argv[]) {
  using lmice::ft_trader_femas2_api_data;
  using lmice::ft_trader_femas2_emulator;
  using lmice::ft_trader_femas2_spi;
  using lmice::ft_trader_femas2_spi_data;

  using lmice::lm_worker_board;

  ft_trader_femas2_spi* spi = nullptr;
  ft_trader_femas2_spi_data* spi_data = nullptr;
  CUstpFtdcTraderApi* api = nullptr;
  ft_trader_femas2_emulator* demo_api = nullptr;
  ft_trader_femas2_api_data* api_data = nullptr;
  char front_address[32] = "tcp://127.0.0.1:8000";

  // create shm board
  lmice_shm_t shm_board;
  board_type::create(&shm_board);
  board_type* board = reinterpret_cast<board_type*>(shm_board.addr);

  g_board = board;

  // create spi
  int fd = board->create_bulk(sizeof(ft_trader_femas2_spi));
  spi = new (board->get_bulk(fd)) ft_trader_femas2_spi;

  // create spi data
  int spi_data_fd = board->create_bulk(sizeof(ft_trader_femas2_spi_data));
  spi_data = new (board->get_bulk(spi_data_fd)) ft_trader_femas2_spi_data;
  memset(spi_data, 0, sizeof(ft_trader_femas2_spi_data));

  spi->set_data(spi_data);

  // create api data
  int api_data_fd = board->create_bulk(sizeof(ft_trader_femas2_api_data));
  api_data = new (board->get_bulk(api_data_fd)) ft_trader_femas2_api_data;
  memset(api_data, 0, sizeof(ft_trader_femas2_api_data));

  // create api
  api = CUstpFtdcTraderApi::CreateFtdcTraderApi();

  // init demo api
  demo_api = dynamic_cast<ft_trader_femas2_emulator*>(api);
  if (demo_api) {
    demo_api->set_board(board);
    demo_api->set_data(api_data);
    demo_api->set_spi_data(spi_data);
    lmice_info_print("init demo api\n");
  }

  api->RegisterSpi(spi);
  api->SubscribePrivateTopic(USTP_TERT_QUICK);
  api->SubscribePublicTopic(USTP_TERT_QUICK);
  //设置心跳超时时间
  api->SetHeartbeatTimeout(5);

  //注册前置机网络地址
  api->RegisterFront(front_address);

  //初始化
  api->Init();

  // 释放 Femas2 api实例
  api->Release();

  // deatroy board
  board->destroy(&shm_board);

  return 0;
}
