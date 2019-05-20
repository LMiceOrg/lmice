/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#ifndef SERVICE_INCLUDE_RES_LIST_H_
#define SERVICE_INCLUDE_RES_LIST_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <iterator>

#define MAU 4096

template <class T>
struct reslist {
  template <class G = T>
  struct reslist_iterator : public std::iterator<std::input_iterator_tag, T> {
    typedef reslist<T> self_type;
    reslist_iterator(int size, int count, int pos, self_type* s) {
      it_pos = pos;
      it_size = size;
      it_count = count;
      self = s;
      it_ptr = self->item + it_pos;
    }
    reslist_iterator& operator=(const reslist_iterator& iter) {
      it_ptr = iter.it_ptr;
      it_pos = iter.it_pos;
      it_size = iter.it_size;
      it_count = iter.it_count;
      self = iter.self;
    }

    bool operator!=(const reslist_iterator& iter) {
      return it_ptr != iter.it_ptr;
    }

    bool operator==(const reslist_iterator& iter) {
      return it_ptr == iter.it_ptr;
    }

    reslist_iterator& operator++() {
      if (it_pos < it_count) {
        ++it_pos;
        it_ptr++;
      }
      if (it_pos == it_count && self->next != NULL) {
        self = self->next;
        it_pos = 0;
        it_ptr = self->item;
      }
      return *this;
    }

    T& operator* () {
        return *it_ptr;
    }

    reslist<T>* self;
    T* it_ptr;
    int it_pos;
    int it_size;
    int it_count;
  };
  typedef reslist_iterator<T> iterator;
  int32_t fd;
  int32_t type;
  int32_t it_size;
  int32_t it_count;
  int32_t depth;

  volatile int64_t lock;
  struct reslist* next;
  T item[1];

  static reslist<T>* create(int count) {
    int size = sizeof(T);
    int bytes = (int64_t)(((reslist<T>*)0)->item) + size * count;
    void* p = malloc(bytes);
    reslist<T>* s = reinterpret_cast<reslist<T>*>(p);
    s->it_size = size;
    s->it_count = count;
    s->depth = 0;
    s->next = NULL;
    return s;
  }

  T& operator[](int index) {
    reslist<T>* p = this;
    int d = index / it_count;
    int pos = index % it_count;

    while (d > 0) {
      p = p->next;
      --d;
    }
    return p->item[pos];
  }

  /* iterator ops */
  iterator begin() { return iterator(it_size, it_count, 0, this); }
  iterator end() {
    reslist<T>* p = this;
    while (p->next != NULL) {
      p = p->next;
      if(p->next == NULL) {
          break;
      }
    }
    return iterator(it_size, it_count, it_count, p);
  }

  int size() { return it_count * (depth + 1); }
};

#endif  // SERVICE_INCLUDE_RES_LIST_H_
