#include <iostream>
#include <string>
#include <vector>

#include <jansson.h> /** Jansson lib */

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../fsimulator/autotools/guavaproto.h"

#include "eal/lmice_eal_time.h"
#include "eal/lmice_trace.h"

#include "include/forecasterimpl.h"

struct pcap_pkghdr {
  unsigned int tv_sec;
  unsigned int tv_usec;
  unsigned int size;
  unsigned int length;
};
#pragma pack(push, 1)
struct pcap_pkgdat {
  char udp_head[42];
  guava_udp_head head;
  union {
    guava_udp_normal normal;
    guava_udp_summary summary;
  };
};

typedef pcap_pkgdat pcap_guava1;

struct pcap_pkgguava2 {
  char udp_head[42];
  SHFEQuotDataT quot;
};

#pragma pack(pop)

#define read_in(pos, sz) fread(buf + pos, sz, 1, fp)
#define read_filehdr() read_in(0, 24)
#define read_pkghdr() read_in(0, 16)
#define read_pkgdat() read_in(16, hdr->size)

typedef struct ssh_config_s {
  int port;
  int authtype;
  std::string host;
  std::string username;
  std::string password;
  std::string root;

} ssh_config_t;

json_t* obj_get(json_t* root, const char* keys) {
  json_t* o = root;
  char* key = NULL;
  size_t nsz = strlen(keys);
  char* nkeys = (char*)malloc(nsz + 1);
  char* last = NULL;
  memcpy(nkeys, keys, nsz);
  nkeys[nsz] = '\0';

  for (key = strtok_r(nkeys, ".", &last); key;
       key = strtok_r(NULL, ".", &last)) {
    /*printf("key:%s\n", key);*/
    o = json_object_get(o, key);
  }

  free(nkeys);
  return o;
}

static inline void get_json_string(json_t* root, const char* key,
                                   std::string& value) {
  json_t* obj = obj_get(root, key);
  if (json_is_string(obj)) {
    value = json_string_value(obj);
  }
}

template <class T>
static inline void get_json_integer(json_t* root, const char* key, T* value) {
  json_t* obj = obj_get(root, key);
  if (json_is_integer(obj)) {
    *value = json_integer_value(obj);
  }
}

std::string& replace_all(std::string& str, const std::string& old_value,
                         const std::string& new_value) {
  while (true) {
    std::string::size_type pos(0);
    if ((pos = str.find(old_value)) != std::string::npos)
      str.replace(pos, old_value.length(), new_value);
    else
      break;
  }
  return str;
}

struct temp_msg {
  bool is_trading;
  ChinaL1Msg msg;
};
typedef std::vector<temp_msg> temp_msg_type;
temp_msg_type temp_msg_list;

/** Implement clib */
template <class Forecaster>
struct in_forecaster_t {
  std::string stype;
  Forecaster* fc;
  std::string trading_instrument;
  std::vector<std::string> subscriptions;
  std::vector<std::string> subsignals;
  // std::vector<FILE*> subfiles;
  FILE* instfp;
  FILE* sigsfp;  // forecast X
  FILE* fcfp;    // forecast Y
  ChinaL1Msg trad_msg;
  std::vector<ChinaL1Msg> sub_msgs;
  bool is_trading;
  bool is_symbol;

  void create(const struct tm& date, const char* type) {
    stype = type;
    fc = new Forecaster();
    fc->init(date);

    //    printf("guava size:%ld\n",
    //           sizeof(guava_udp_head) + sizeof(guava_udp_normal));
    //    printf("SHFEQuotDataT size%ld\n", sizeof(SHFEQuotDataT));
    printf("begin create...\n");
    trading_instrument = fc->get_trading_instrument();

    prod_string(trad_msg.m_inst, trading_instrument.c_str());
    subscriptions = fc->get_subscriptions();

    for (size_t i = 0; i < subscriptions.size(); ++i) {
      ChinaL1Msg msg;

      prod_string(msg.m_inst, subscriptions[i].c_str());
      sub_msgs.push_back(msg);
      lmice_info_print("sub [%lu] = %s\n", i, subscriptions[i].c_str());
    }
    subsignals = fc->get_subsignal_name();
  }

  void close() { delete fc; }

  void write_signalindex(const std::string& name) {
    // printf("index file name: %s\n", name.c_str());
    FILE* fidx = fopen(name.c_str(), "w");
    fprintf(fidx,
            "{\n"
            "\"size\":%ld,\n"
            "\"names\":[\n",
            subsignals.size());
    size_t i;
    if (subsignals.size() > 0) {
      fprintf(fidx, "\t\"%s\"\n", subsignals[0].c_str());
    }
    for (i = 1; i < subsignals.size(); ++i) {
      fprintf(fidx, "\t,\"%s\"\n", subsignals[i].c_str());
    }
    fprintf(fidx, "],\n");

    const float* weight = NULL;
    int size = 0;
    fc->get_all_weight(&weight, &size);
    fprintf(fidx, "\"weights\":[\n");
    if (size > 0) {
      fprintf(fidx, "\t%016.014lf\n", weight[0]);
    }
    for (int idx = 1; idx < size; ++idx) {
      fprintf(fidx, "\t,%016.014lf\n", weight[idx]);
    }
    fprintf(fidx, "]\n}\n");

    fclose(fidx);
  }

  void write_instrument(FILE* fp, const ChinaL1Msg& msg_data) {
    /* Total 68 bytes:  instrument -- 8 bytes  data -- 60 bytes
     */
    fwrite(&msg_data, 1, sizeof(ChinaL1Msg), fp);
  }

  inline void write_subsignals(const ChinaL1Msg& msg_data) {
    const float* rdata;
    int rsize = 0;
    int i;
    double signal;
    FILE* fp = sigsfp;

    signal = fc->get_forecast();
    // fc->get_all_signal(&rdata, &rsize);
    return;
    double forecast = fc->get_forecast1();
    // Write subsignals
    fprintf(fp, "\"%lld\", %016.014lf, %016.014lf", msg_data.m_time_micro,
            signal, forecast);

    for (i = 0; i < rsize; ++i) {
      fprintf(fp, ",%016.014lf", *(rdata + i));
    }
    fprintf(fp, "\n");

    // Write forecast
    fwrite(rdata, 1, sizeof(double) * rsize, fcfp);
  }
  bool prepare_proc_l1msg(const ChinaL1Msg& msg_data) {
    return fc->pre_proc_msg(msg_data, 0);
  }
  void proc_l1msg(const ChinaL1Msg& msg_data) {
    // Calc signals
    fc->update(msg_data);

    if (!is_trading) return;

    // Save ChinaL1Msg
    // write_instrument(instfp, msg_data);

    // Save signals
    write_subsignals(msg_data);
  }

  int calc_subsignals(const char* dtfile, const char* outpath,
                      const char* namepattern) {
    char buf[256];
    FILE* fp;

    fp = fopen(dtfile, "rb");
    if (!fp) return EBADF;

    std::string sdate;
    std::string stime;
    {
      std::string dfile = dtfile;
      std::string::size_type pos(0);
      pos = dfile.find("__", 0);
      if (pos == std::string::npos) {
        printf("filename havn't __ splited date and time [%s]\n", dtfile);
        return 0;
      }
      sdate = dfile.substr(pos - 10, 10);
      stime = dfile.substr(pos + 2, 8);
    }

    {
      std::string name = namepattern;
      name = replace_all(name, "%type", stype);

      name = replace_all(name, "%date", sdate);
      name = replace_all(name, "%time", stime);
      name = "/" + name;
      name = outpath + name + ".csv";

      // printf("sig file name :%s\n", name.c_str());
      sigsfp = fopen(name.c_str(), "w");

      fprintf(sigsfp, "\"Date\",\"Signal\"");
      for (size_t i = 0; i < subsignals.size(); ++i) {
        fprintf(sigsfp, ",\"%s\"", subsignals[i].c_str());
      }
      fprintf(sigsfp, "\n");

      name.replace(name.end() - 4, name.end(), ".dat");
      // printf("inst file name: %s\n", name.c_str());
      instfp = fopen(name.c_str(), "wb");

      name.replace(name.end() - 4, name.end(), ".fc");
      // printf("forecast file name: %s\n", name.c_str());
      fcfp = fopen(name.c_str(), "w");

      /// subsignal index file
      name.replace(name.end() - 3, name.end(), ".idx");
      write_signalindex(name);
    }

    const char* p = strstr(dtfile, "guava2");
    if (!p) {
      proc_guava1(buf, fp);
    } else {
      proc_guava2(buf, fp);
    }

    fclose(fp);
    fclose(sigsfp);
    fclose(instfp);
    fclose(fcfp);

    return 0;
  }

  void proc_guava1(char* buf, FILE* fp) {
    pcap_pkghdr* hdr = reinterpret_cast<pcap_pkghdr*>(buf);
    pcap_guava1* data = reinterpret_cast<pcap_guava1*>(buf + 16);
    ChinaL1Msg* pmsg_data;
    const uint64_t dmax = 0x7fefffffffffffff;
    /* file header */
    read_filehdr();
    do {
      /* package header */
      read_pkghdr();

      /* package data */
      read_pkgdat();

      if (hdr->size == 42 + sizeof(guava_udp_head) + sizeof(guava_udp_normal)) {
        is_symbol = false;
        is_trading = false;
        if (strcmp(trading_instrument.c_str(), data->head.m_symbol) == 0) {
          is_symbol = true;
          is_trading = true;
          pmsg_data = &trad_msg;
        }
        for (size_t i = 0; i < subscriptions.size(); ++i) {
          if (strcmp(subscriptions[i].c_str(), data->head.m_symbol) == 0) {
            is_symbol = true;
            pmsg_data = &sub_msgs[i];
            break;
          }
        }

        if (!is_symbol) {
          continue;
        }
        // printf("%s. %s\n", in->trading_instrument.c_str(),
        // data->head.m_symbol);

        if ((data->normal.m_ask_pxi != dmax) &&
            (data->normal.m_bid_pxi != dmax) &&
            (data->normal.m_last_pxi != dmax)) {
          /** Calculate */
          ChinaL1Msg& msg_data = *pmsg_data;
          int64_t micro_time = (int64_t)hdr->tv_sec * 1000000LL +
                               data->head.m_millisecond * 1000LL;

          // msg_dt.m_inst = data->head.m_symbol;
          // msg_dt.m_time_micro = dtime*1000000+data->head.m_millisecond*1000;
          msg_data.m_time_micro = micro_time;

          msg_data.m_limit_up = 0;
          msg_data.m_limit_down = 0;

          switch ((int)data->head.m_quote_flag) {
            case 1:
              // msg_dt.m_bid = data->normal.m_bid_px;
              // msg_dt.m_offer = data->normal.m_ask_px;
              // msg_dt.m_bid_quantity = data->normal.m_bid_share;
              // msg_dt.m_offer_quantity = data->normal.m_ask_share;
              msg_data.m_volume = data->normal.m_last_share;
              // guava1 no-data md_data->PriVol.Volume ;
              msg_data.m_notional = data->normal.m_total_value;
              // md_data->PriVol.Turnover;
              break;
            case 2:
              msg_data.m_bid = data->normal.m_bid_px;
              msg_data.m_offer = data->normal.m_ask_px;
              msg_data.m_bid_quantity = data->normal.m_bid_share;
              msg_data.m_offer_quantity = data->normal.m_ask_share;
              // msg_dt.m_volume = 0;//guava1 no-data md_data->PriVol.Volume ;
              // msg_dt.m_notional =
              // data->normal.m_total_pos;//md_data->PriVol.Turnover;
              break;
            case 3:
              msg_data.m_bid = data->normal.m_bid_px;
              msg_data.m_offer = data->normal.m_ask_px;
              msg_data.m_bid_quantity = data->normal.m_bid_share;
              msg_data.m_offer_quantity = data->normal.m_ask_share;
              msg_data.m_volume = data->normal.m_last_share;
              // guava1 no-data md_data->PriVol.Volume ;
              msg_data.m_notional = data->normal.m_total_value;
              // md_data->PriVol.Turnover;
              break;
            case 0:
              break;
            case 4:
              continue;
          }

          proc_l1msg(msg_data);
        }
      }

    } while (feof(fp) == 0);
  }

  void proc_guava2(char* buf, FILE* fp) {
    pcap_pkghdr* hdr = reinterpret_cast<pcap_pkghdr*>(buf);
    pcap_pkgguava2* data = reinterpret_cast<pcap_pkgguava2*>(buf + 16);
    ChinaL1Msg* pmsg_data;
    const uint64_t dmax = 0x7fefffffffffffff;
    /* file header */
    read_filehdr();
    temp_msg_list.clear();
    temp_msg_list.reserve(204800);
    for (;;) {
      /* package header */
      if (feof(fp) != 0) break;
      read_pkghdr();
      // break;

      //        printf("sec :%u, usec:%u\n", hdr->tv_sec, hdr->tv_usec);
      //        if(cnt > 100)
      //            break;

      if (hdr->size != sizeof(pcap_pkgguava2)) {
        fseek(fp, hdr->size, SEEK_CUR);
        continue;
      }

      read_pkgdat();

      // SHFEQuotDataTag* s = reinterpret_cast<SHFEQuotDataTag*>(buf);
      // printf("%s, %s\t%d:\tsz:%d\t in1 : %s\t in2: %s\n", cnt, sz,
      // data->head.m_symbol, s->InstrumentID);

      is_symbol = false;
      is_trading = false;
      if (strcmp(trading_instrument.c_str(), data->quot.InstrumentID) == 0) {
        is_symbol = true;
        is_trading = true;
        pmsg_data = &trad_msg;
      } else {
        for (size_t i = 0; i < subscriptions.size(); ++i) {
          if (strcmp(subscriptions[i].c_str(), data->quot.InstrumentID) == 0) {
            is_symbol = true;
            pmsg_data = &sub_msgs[i];
            break;
          }
        }
      }
      //      if (strcmp(data->quot.InstrumentID, "ni1709") == 0)
      //        printf("got ni1709 %d\n", is_symbol);
      // printf("done %d %d \n", is_symbol, feof(fp));

      if (!is_symbol) {
        continue;
      }

      if ((data->quot.AskPrice1i != dmax) && (data->quot.BidPrice1i != dmax) &&
          (data->quot.LastPricei != dmax)) {
        /** Calculate */
        ChinaL1Msg& msg_data = *pmsg_data;
        int64_t micro_time = 0;
        time_t current;
        struct tm t;
        current = time(NULL);
        localtime_r(&current, &t);

        //        char data_time[16];
        //        memset(data_time, 0, 16);
        //        strncpy(data_time, data->quot.UpdateTime, 8);
        //        char tmp[8];
        //        memset(tmp, 0, 8);
        //        memcpy(tmp, data_time, 2);

        //        t.tm_hour = atoi(tmp);
        //        memcpy(tmp, data_time + 3, 2);
        //        t.tm_min = atoi(tmp);
        //        memcpy(tmp, data_time + 6, 2);
        //        t.tm_sec = atoi(tmp);

        t.tm_hour = (data->quot.UpdateTime[0] - '0') * 10 +
                    (data->quot.UpdateTime[1] - '0');
        t.tm_min = (data->quot.UpdateTime[3] - '0') * 10 +
                   (data->quot.UpdateTime[4] - '0');
        t.tm_sec = (data->quot.UpdateTime[6] - '0') * 10 +
                   (data->quot.UpdateTime[7] - '0');

        micro_time = mktime(&t);

        micro_time = micro_time * 1000000LL +
                     data->quot.UpdateMillisec * 1000LL;  // hdr->tv_usec;

        // Update ChinaL1Msg
        msg_data.m_time_micro = micro_time;

        msg_data.m_limit_up = 0;
        msg_data.m_limit_down = 0;

        msg_data.m_bid = data->quot.BidPrice1;
        msg_data.m_offer = data->quot.AskPrice1;
        msg_data.m_bid_quantity = data->quot.BidVolume1;
        msg_data.m_offer_quantity = data->quot.AskVolume1;
        msg_data.m_volume = data->quot.Volume;      // Volume ;
        msg_data.m_notional = data->quot.Turnover;  // Turnover;

        // sanity check the quotes
        if (msg_data.m_bid_quantity <= 0) {
          msg_data.m_bid = 0;
        }

        if (msg_data.m_offer_quantity <= 0) {
          msg_data.m_offer = 0;
        }

        temp_msg m;
        m.is_trading = is_trading;
        memcpy(&m.msg, &msg_data, sizeof(msg_data));

        temp_msg_list.push_back(m);
        //        proc_l1msg(msg_data);
        //        if (m_pnt_count < 4 && strcmp(msg_data.get_inst(), "ni1709")
        //        == 0) {
        //          m_pnt_count++;
        //          printf("ni[%d]=\t%lf,%lf,%d,%d\n", m_pnt_count,
        //          msg_data.m_bid,
        //                 msg_data.m_offer, msg_data.m_bid_quantity,
        //                 msg_data.m_offer_quantity);
        //        }
      }
    }  // end-for
    int64_t t1;
    int64_t t2;
    int cnt = 1;

    size_t i = 0;
    for (i = 0; i < temp_msg_list.size(); ++i) {
      ChinaL1Msg& msg_data = temp_msg_list[i].msg;
      is_trading = temp_msg_list[i].is_trading;
      int ret = fc->pre_proc_msg(msg_data);
      if (ret == 1) break;

      // if (cnt >= 10) break;
    }
    i++;
    // printf("i= %zu size %zu\n", i, temp_msg_list.size());
    get_tick_count(&t1);
    for (; i < temp_msg_list.size(); ++i) {
      ChinaL1Msg& msg_data = temp_msg_list[i].msg;
      is_trading = temp_msg_list[i].is_trading;
      proc_l1msg(msg_data);
      if (is_trading) ++cnt;
    }

    get_tick_count(&t2);
    lmice_critical_print("total proc time %lld, size %d, avg: %lf\n", t2 - t1,
                         cnt, ((double)(t2 - t1)) / cnt);
  }
};

enum e_order_state { ST_ASK = 1 << 0, ST_BID = 1 << 1, ST_INSERT = 1 << 2 };

struct st_order_state {
  int cur_order_state;
  double cur_order_px;
  double his_order_px;
  int64_t cur_order_tm;
  int ask_count;
  int bid_count;
  int sound_ask_count;
  int sound_bid_count;
};

int get_date(struct tm& date, const char* name) {
  const char* bsep = "__";
  const char* bpos;
  const char* sdate;

  memset(&date, 0, sizeof(struct tm));

  bpos = strstr(name, bsep);
  sdate = bpos - 10;
  if (bpos == NULL) {
    return 1;
  }

  date.tm_year = (sdate[0] - '0') * 1000 + (sdate[1] - '0') * 100 +
                 (sdate[2] - '0') * 10 + (sdate[3] - '0') - 1900;

  date.tm_mon = (sdate[5] - '0') * 10 + (sdate[6] - '0') - 1;

  date.tm_mday = (sdate[8] - '0') * 10 + (sdate[9] - '0');

  return 0;
}
#define forecaster_proc()                                        \
  in_forecaster_t<fc_type>* in = new in_forecaster_t<fc_type>(); \
  in->create(date, insttype);                                    \
  in->calc_subsignals(file.c_str(), outpath, namepattern);       \
  in->close();                                                   \
  delete in;

int fc_calc(const char* cfg) {
  json_t* root = NULL;
  json_error_t err;
  json_t* obj = NULL;
  const char* key = NULL;
  const char* value = NULL;

  const char* insttype = NULL;

  json_t* arr = NULL;
  size_t i;

  /** Load json conf */
  root = json_load_file(cfg, 0, &err);
  if (root == NULL) {
    printf("load config file error:%s at line %d, col %d\n", err.text, err.line,
           err.column);
    return -1;
  }

  /** Forecaster type */
  key = "insttype";
  obj = json_object_get(root, key);
  if (!json_is_string(obj)) {
    printf("config key s[%s] is missing.\n", key);
    return -1;
  }
  value = json_string_value(obj);
  insttype = value;

  /** Output path */
  key = "signal_calc.outpath";
  obj = obj_get(root, key);
  if (!json_is_string(obj)) {
    printf("config key s[%s] is missing.\n", key);
    return -1;
  }
  const char* outpath = json_string_value(obj);

  /** Name pattern */
  key = "signal_calc.pattern";
  obj = obj_get(root, key);
  if (!json_is_string(obj)) {
    printf("config key s[%s] is missing.\n", key);
    return -1;
  }
  const char* namepattern = json_string_value(obj);

  /** Data path */
  key = "signal_calc.datpath";
  obj = obj_get(root, key);
  if (!json_is_string(obj)) {
    printf("config key s[%s] is missing.\n", key);
    return -1;
  }
  const char* datapath = json_string_value(obj);

  /** Data list */
  key = "signal_calc.files";
  obj = obj_get(root, key);
  if (!json_is_array(obj)) {
    printf("config key s[%s] is missing.\n", key);
    return -1;
  }

  arr = obj;

  for (i = 0; i < json_array_size(arr) && (obj = json_array_get(arr, i)); i++) {
    if (!json_is_string(obj)) {
      printf("config key s[%s] is misstype.\n", key);
      return -1;
    }
    value = json_string_value(obj);

    std::string file = datapath;
    file.append("/");
    file.append(value);

    struct tm date;
    get_date(date, value);

    lmice_critical_print("proc %s\n", file.c_str());

    /*    if (strcmp(insttype, "rb_0") == 0) {
          typedef lmice::RB_0_ForecasterMP fc_type;
          forecaster_proc();
        } else if (strcmp(insttype, "hc_0") == 0) {
          typedef lmice::HC_0_Forecaster fc_type;
          forecaster_proc();
        } else */
    if (strcmp(insttype, "cu1_0") == 0) {
      {
        typedef cu1_0_forecaster fc_type;
        forecaster_proc();
      }
      //      {
      //        typedef lmice::CU1_0_ForecasterMP fc_type;
      //        forecaster_proc();
      //      }
    }
    lmice_critical_print("proc done\n");
  }

  return 0;
}

int main(int argc, char* argv[]) {
  lmice_critical_print("Begin signal calc, argc = %d\n", argc);
  if (argc == 2) {
    fc_calc(argv[1]);
  }

  return 0;
}
