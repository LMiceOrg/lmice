/** Copyright 2018 He Hao<hehaoslj@sina.com> */


#include "include/res_manager.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
static void* posix_shm_create(const char* name, int size);
static void* posix_shm_open(const char* name);
static int posix_shm_close(void* ad);

void* posix_shm_create(const char* name, int size) {
  int fd;
  void* ad = NULL;

  fd = shm_open(name, O_RDWR | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) return NULL;

  ftruncate(fd, size);

  ad = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if (MAP_FAILED == ad) return NULL;

  return ad;
}

void* posix_shm_open(const char* name) {
  int fd;
  int size;
  int ret;
  struct stat st;
  void* ad = NULL;

  fd = shm_open(name, O_RDWR);
  if (fd < 0) return NULL;

  st.st_size = 0;
  ret = fstat(fd, &st);
  if (ret != 0) return NULL;
  size = st.st_size;

  ad = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if (MAP_FAILED == ad) return NULL;

  return ad;
}

int posix_shm_close(void* ad) {
  int fd;
  int size;
  const char* name;

  munmap(ad, size);
  close(fd);
  shm_unlink(name);
}
