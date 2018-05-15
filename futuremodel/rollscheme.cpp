/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include "include/rollscheme.h"

#include <immintrin.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include "include/featurebase.h"
#include "include/scheme.h"

forceinline bool operator==(const product_alias& pa1,
                            const product_alias& pa2) {
  int value;
  value = strcmp(pa1.alias, pa2.alias);
  if (value != 0) return false;
#if 0
  int idate1 = 0;
  int idate2 = 0;
  idate1 += (pa1.date[0] - '0') * 10000000 + (pa1.date[1] - '0') * 1000000 +
            (pa1.date[2] - '0') * 100000 + (pa1.date[3] - '0') * 10000 +
            (pa1.date[4] - '0') * 1000 + (pa1.date[5] - '0') * 100 +
            (pa1.date[6] - '0') * 10 + (pa1.date[7] - '0') * 1;
  idate2 += (pa2.date[0] - '0') * 10000000 + (pa2.date[1] - '0') * 1000000 +
            (pa2.date[2] - '0') * 100000 + (pa2.date[3] - '0') * 10000 +
            (pa2.date[4] - '0') * 1000 + (pa2.date[5] - '0') * 100 +
            (pa2.date[6] - '0') * 10 + (pa2.date[7] - '0') * 1;
  return idate1 == idate2;
#endif
#if 1
  return (*(const int64_t*)pa1.date) == (*(const int64_t*)pa2.date);

#endif
}

forceinline bool operator<(const product_alias& pa1, const product_alias& pa2) {
  int value;

  value = strcmp(pa1.alias, pa2.alias);
  if (value != 0) return false;
#if 0
  int idate1 = 0;
  int idate2 = 0;
  idate1 += (pa1.date[0] - '0') * 10000000 + (pa1.date[1] - '0') * 1000000 +
            (pa1.date[2] - '0') * 100000 + (pa1.date[3] - '0') * 10000 +
            (pa1.date[4] - '0') * 1000 + (pa1.date[5] - '0') * 100 +
            (pa1.date[6] - '0') * 10 + (pa1.date[7] - '0') * 1;
  idate2 += (pa2.date[0] - '0') * 10000000 + (pa2.date[1] - '0') * 1000000 +
            (pa2.date[2] - '0') * 100000 + (pa2.date[3] - '0') * 10000 +
            (pa2.date[4] - '0') * 1000 + (pa2.date[5] - '0') * 100 +
            (pa2.date[6] - '0') * 10 + (pa2.date[7] - '0') * 1;
return idate1 < idate2;
#endif
#if 1
  return (*(const int64_t*)pa1.date) < (*(const int64_t*)pa2.date);

#endif
}

namespace lmice {
static product_item product_list[] = {
    /* product  tick contract */
    {"cu", 10, 5}, {"au", 0.05, 1000}, {"al", 5, 5},  {"fu", 1, 50},
    {"zn", 5, 5},  {"rb", 1, 10},      {"ru", 5, 10}, {"pb", 5, 5},
    {"bu", 2, 10}, {"ag", 1, 15},      {"hc", 1, 10}, {"ni", 10, 1},
    {"sn", 10, 1}, {"j", 1, 100},      {"jm", 1, 60}, {"i", 1, 100},
    {"a", 1, 10},  {"m", 1, 10},       {"y", 2, 10},  {"p", 2, 10},
    {"l", 5, 5},   {"pp", 1, 5},       {"v", 5, 5},   {"", 0, 0}};

double roll_scheme::get_ticksize(const std::string& prod) {
  size_t i;
  for (i = 0; i < sizeof(product_list) / sizeof(product_item); ++i) {
    const product_item& item = product_list[i];
    size_t size = strlen(item.id);
    if (strncmp(prod.c_str(), item.id, size) == 0) {
      return item.tick_size;
    }
  }
  printf("get_ticksize: unknown product id\n");
  return 0;
}

double roll_scheme::get_constract_size(const std::string& prod) {
  size_t i;
  for (i = 0; i < sizeof(product_list) / sizeof(product_item); ++i) {
    const product_item& item = product_list[i];
    size_t size = strlen(item.id);
    if (strncmp(prod.c_str(), item.id, size) == 0) {
      return item.constract_size;
    }
  }
  printf("get_constract_size: product is not defined\n");
  return 0;
}

std::string roll_scheme::from_alias(const std::string& alias,
                                    const struct tm& date) {
  int value;
  char sdata[10];
  memset(sdata, 0, sizeof(sdata));

  value = date.tm_year + 1900;
  sdata[0] = '0' + (value % 10000) / 1000;
  sdata[1] = '0' + (value % 1000) / 100;
  sdata[2] = '0' + (value % 100) / 10;
  sdata[3] = '0' + (value % 10);

  value = date.tm_mon + 1;
  sdata[4] = '0' + (value % 100) / 10;
  sdata[5] = '0' + (value % 10);

  value = date.tm_mday;
  sdata[6] = '0' + (value % 100) / 10;
  sdata[7] = '0' + (value % 10);

  product_alias key;
  memset(&key, 0, sizeof(key));
  strncpy(key.alias, alias.c_str(), alias.length());
  memcpy(key.date, sdata, sizeof(key.date));

  const product_alias* pa;
  const product_alias* pa_end =
      product_aliases + sizeof(product_aliases) / sizeof(product_alias);
  const product_alias* pa_begin = product_aliases;
  pa = std::find(pa_begin, pa_end, key);
  if (pa == pa_end) {
    // printf("not found item(%s), %s try upper_bound\n", key.alias, key.date);

    const product_alias* last_pa = pa_end;
    int64_t id2 = (*(const int64_t*)key.date);
    id2 = __builtin_bswap64(id2);

    for (pa = pa_begin; pa != pa_end; ++pa) {
      int64_t id1 = (*(const int64_t*)pa->date);
      id1 = __builtin_bswap64(id1);
      if (id1 > id2) break;

      if (strcmp(pa->alias, key.alias) == 0) last_pa = pa;
    }
    pa = last_pa;
    if (pa == pa_end) throw "find alias failed.";
  }

  // printf("find alias = %s, date = %s, inst =%s\n", pa->alias, pa->date,
  //      pa->instrument_id);

  return pa->instrument_id;
}

void roll_scheme::from_alias2(product_id* id, const std::string& alias,
                              const tm& date) {
  std::string prod_id = from_alias(alias, date);
  prod_string(id, prod_id.c_str());
}
}  // namespace lmice
