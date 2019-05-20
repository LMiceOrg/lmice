/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef SERVICE_INCLUDE_LOG_H_
#define SERVICE_INCLUDE_LOG_H_

#include "include/server.h"

int log_open(server_t* srv);

int log_close(int fd);

#endif  // SERVICE_INCLUDE_LOG_H_
