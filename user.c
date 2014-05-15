#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "mypthread.h"
#include "mymutex.h"

static mypthread_t t1, t2;
static mypthread_mutex_t mutex;
static int *a;

void *bar(void *arg) {
  if (mypthread_mutex_trylock(&mutex)) {
    printf("bar couldn't get lock\n");
  } else {
    printf("bar got lock\n");
  }

  return NULL;
}

void *foo(void *arg) {
  if (mypthread_mutex_lock(&mutex)) {
    printf("foo couldn't get lock\n");
  } else {
    printf("foo got lock\n");
    *a = 15;
    sleep(1);
  }

  return NULL;
}

int main() {
  a = malloc(sizeof(int));

  mypthread_mutex_init(&mutex);

  mypthread_create(&t1, foo, NULL);
  mypthread_create(&t2, bar, NULL);


  mypthread_join(t1, NULL);

  free(a);
  return 0;
}

