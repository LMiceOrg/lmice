#include "eal/lmice_eal_time.h"

/** service */
#include "include/service_logging.h"

#include "include/trader_femas2_emulator.h"
#include "include/trader_femas2_spi.h"

typedef lmice::lm_worker_board board_type;

board_type* g_board = nullptr;

/** global function */
void print_usage(void);

int main(int argc, char* argv[]) {
  using api_data_t = lmice::ft_trader_femas2_api_data;
  using api_emu_t = lmice::ft_trader_femas2_emulator;
  using spi_t = lmice::ft_trader_femas2_spi;
  using spi_data_t = lmice::ft_trader_femas2_spi_data;

  using lmice::lm_worker_board;

  spi_t* spi = nullptr;
  spi_data_t* spi_data = nullptr;
  CUstpFtdcTraderApi* api = nullptr;
  api_emu_t* demo_api = nullptr;
  api_data_t* api_data = nullptr;
  char front_address[32] = "tcp://127.0.0.1:8000";

  lmice_shm_t shm_board;

  (void)argc;
  (void)argv;

  // create shm board

  board_type::create(&shm_board);
  board_type* board = reinterpret_cast<board_type*>(shm_board.addr);

  g_board = board;

  // create spi
  int fd = board->create_bulk(sizeof(spi_t));
  spi = new (board->get_bulk(fd)) spi_t;

  // create spi data
  int spi_data_fd = board->create_bulk(sizeof(spi_data_t));
  spi_data = new (board->get_bulk(spi_data_fd)) spi_data_t;
  memset(spi_data, 0, sizeof(spi_data_t));

  spi->set_data(spi_data);

  // create api
  api = CUstpFtdcTraderApi::CreateFtdcTraderApi();

  // init demo api
  demo_api = dynamic_cast<api_emu_t*>(api);
  if (demo_api) {
    // create api data
    int api_data_fd = board->create_bulk(sizeof(api_data_t));
    api_data = new (board->get_bulk(api_data_fd)) api_data_t;
    memset(api_data, 0, sizeof(api_data_t));

    demo_api->set_board(board);
    demo_api->set_data(api_data);
    demo_api->set_spi_data(spi_data);
    lmice_info_print("init demo api\n");
  }
  lmice_info_print("demo api %p\n", demo_api);

  api->RegisterSpi(spi);

  // 订阅
  //        TERT_RESTART:从本交易日开始
  //        TERT_RESUME:从上次收到的续传
  //        TERT_QUICK:只传送登录后私有流的内容
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

  unsigned high[20];
  unsigned low[20];
  unsigned long long value[10];
  // uint64_t t1, t2;
  printf("read begin\n");
  for (int i = 0; i < 10; ++i) {
    // double x = i + 1.0;
    eal_atomic_cycle_begin(high + i * 2, low + i * 2);
    //    x *= x;
    //    x *= x;
    //    x *= x;
    // printf("read begin 2\n");
    // eal_atomic_cycle_end(high + 1, low + 1);
    eal_atomic_cycle_begin(high + i * 2 + 1, low + i * 2 + 1);
    // printf("read end\n");
    value[i] = eal_atomic_cycle_value(high + i * 2, low + i * 2);
  }
  for (int i = 0; i < 10; ++i) {
    lmice_info_print("zero cycle value %llu\n", value[i]);
  }

  // deatroy board
  board->destroy(&shm_board);

  return 0;
}

void print_usage(void) {
  printf(
      "TraderFemas2 -- a trader testing app --\n\n"
      "\t-h, --help\t\tshow this message\n"
      "\t-n, --name\t\tset module name\n"
      "\t-d, --device\t\tset adapter device name\n"
      "\t-u, --uid\t\tset user id when running\n"
      "\t-s, --silent\t\trun in silent mode[backend]\n"
      "\t-f, --filter\t\tfilter to run pcap\n"
      "\t-p, --position\t\tposition to catch symbol\n"
      "\t-b, --bytes\t\tpackage size limitation\n"
      "\t-m, --multicast\t\tmulticast group, bind ip, port\n"
      "\t-c, --cpuset\t\tSet cpuset for this app(, separated)\n"
      "\t-t, --test\t\tUnit test\n"

      "strategyht: -- strategyht app --\n\n"
      "\t-sv, --svalue\t\tyesterday value of corresponding account\n"
      "\t-sp, --sposition\t\tmax position can hold\n"
      "\t-sl, --sloss\t\tmax loss can make\n"

      "fm2trader: -- Femas2.0 Trader app --\n\n"
      "\t-tf, --tfront\t\tset front address\n"
      "\t-tu, --tuser\t\tset user id \n"
      "\t-tp, --tpassword\t\tset password\n"
      "\t-tb, --tbroker\t\tset borker id\n"
      "\t-te, --texchange\t\tset exchange id\n"
      "\n");
}
