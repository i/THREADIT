#ifndef MYMUTEX_H
#define MYMUTEX_H
#include "mypthread.h"

#define FREE 0
#define LOCKED 1

typedef unsigned int uint;

typedef struct mypthread_mutex {
  uint state;
  int ready;
} mypthread_mutex_t;

int mypthread_mutex_init(mypthread_mutex_t *);
int mypthread_mutex_lock(mypthread_mutex_t *);
int mypthread_mutex_trylock(mypthread_mutex_t *);
int mypthread_mutex_unlock(mypthread_mutex_t *);
int mypthread_mutex_destroy(mypthread_mutex_t *);

static inline unsigned int xchg(uint *addr, uint newval) {
  uint result;
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

#endif
