/** Copyright 2018 He Hao<hehaoslj@sina.com> */
#ifndef SERVICE_INCLUDE_UTILS_H_
#define SERVICE_INCLUDE_UTILS_H_

static inline int str_int(const char* str, int pos, int len) {
  const char* p = str + pos;
  int v = 0;
  int sign = 1;
  int i = 1;

  if (*p == 0x2d) {  /* - */
    sign = -1;
    ++p;
  } else if (*p == 0x2b) {  /* + */
    sign = 1;
    ++p;
  }

  for (i = 1; i < len; ++i) {
    if (p) {
      if (*p == 0x2c)  /* , */
        continue;
      else if (*p < 0x30 || *p > 0x39)  /* 0 - 9 */
        break;
      v = v * 10 + (*p - 0x30);
      ++p;
    }
  }
  return v * sign;
}

#define str_equal(x, y) (strncmp((x), (y), sizeof(y)) == 0)

#endif  /* SERVICE_INCLUDE_UTILS_H_ */
