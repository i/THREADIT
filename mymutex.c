#include "mymutex.h"


int mypthread_mutex_init(mypthread_mutex_t *mutex) {
  if (mutex->ready) {
    return -1;
  }

  mutex->ready = 1;
  mutex->state = FREE;
  return 0;
}

int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
  if (mutex->ready) {
    while (xchg(&mutex->state, 1) != 0) ;
    return 0;
  }
  return -1;
}

int mypthread_mutex_trylock(mypthread_mutex_t *mutex) {
  if (mutex->ready)
    return xchg(&mutex->state, 1);
  return -1;
}

int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
  if (mutex->ready)
    return !xchg(&mutex->state, 0);
  return -1;
}

int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
  if (mutex->ready) {
    mutex->ready = 0;
    return 0;
  }
  return -1;
}
