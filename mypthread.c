#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "mypthread.h"

#define start_t void(*)(void)

// create pthread, add it to thread table
int mypthread_create(mypthread_t *thread, void *(*start_routine) (void *), void *arg)
{
  int i, first = 0;

  if (!tableup) {
    ttable_init();
    start_timer();
    first = 1;
  }

  // set up context
  getcontext(&thread->ctx);
  thread->ctx.uc_stack.ss_sp = calloc(STACK_SZ, 1);
  thread->ctx.uc_stack.ss_size = STACK_SZ;
  thread->ctx.uc_link = NULL;
  makecontext(&thread->ctx, (start_t)start, 1, thread);

  thread->arg = arg;
  thread->fn = start_routine;
  thread->state = RUNNABLE;
  thread->join_id = 0;
  thread->retval = NULL;

  for (i = 1; i < MAX_THREADS; i++) {
    if (ttable[i] == NULL || ttable[i]->state == UNUSED) {
      ttable[i] = thread;
      thread->id = i;
      if (first) {
        // set up main() thread
        ttable[0]->state = RUNNABLE;
        ttable[0]->id = 0;
        ttable[0]->retval = NULL;
        ttable[0]->join_id = 0;
        getcontext(&ttable[0]->ctx);
      }
      return 0;
    }
  }

  return -1;
}

void mypthread_exit(void *retval)
{
  mypthread_t *curr = ttable[pos];
  curr->retval = retval;
  curr->state = TERMINATED;
  mypthread_yield();
}

// give up control of processor
int mypthread_yield(void)
{
  mypthread_t *old, *new;
  old = ttable[pos];
  new = next_thread();
  // printf("Swapping out thread %d with %d\n", old->id, new->id);
  pos = new->id;

  return swapcontext(&old->ctx, &new->ctx);
}

// wait for thread thr to exit
int mypthread_join(mypthread_t thr, void **retval)
{
  mypthread_t *curr = ttable[pos];
  mypthread_t *joinee = ttable[thr.id];
  curr->join_id = thr.id;
  curr->state = WAITING;

  while (ttable[thr.id]->state != TERMINATED) {
    mypthread_yield();
  }
  curr->state = RUNNABLE;

  if (retval) {
    *retval = joinee->retval;
  }

  return 0;
}

// set up the thread table
void ttable_init(void)
{
  int i;
  tableup = 1;
  for (i = 0; i < MAX_THREADS; i++) {
    ttable[i] = NULL;
  }
  ttable[0] = &main_thread;
}

void start_timer(void)
{
  struct sigaction sa;
  struct itimerval timer;

  sa.sa_handler = handler;
  sigaction(SIGVTALRM, &sa, NULL);

  timer.it_interval.tv_sec = 1;
  timer.it_interval.tv_usec = 0;
  timer.it_value.tv_sec = 1;
  timer.it_value.tv_usec = 0;
  setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void handler(int sig) {
  mypthread_yield();
}

// get next runnable thread
mypthread_t *next_thread(void)
{
  int i = pos + 1;
  for (;; i++) {
    i = i % MAX_THREADS;
    if (ttable[i] != NULL) {
      if (ttable[i]->state == RUNNABLE) {
        return ttable[i];
      }
      if (ttable[i]->state == WAITING &&
          ttable[ttable[i]->join_id]->state == TERMINATED) {
          return ttable[i];
      }
    }
  }
}

void print_thread_table()
{
  int i;
  for (i = 0; i < MAX_THREADS; i++) {
    if (ttable[i] != NULL) {
      printf("{ id: %d, state: ", i);
      switch (ttable[i]->state) {
        case UNUSED:
          printf("UNUSED");
          break;
        case RUNNABLE:
          printf("RUNNABLE");
          break;
        case WAITING:
          printf("WAITING[%d]", ttable[i]->join_id);
          break;
        case BLOCKED:
          printf("BLOCKED");
          break;
        case TERMINATED:
          printf("TERMINATED");
          break;
      }
      if (ttable[i]->state == TERMINATED)
        printf(", retval: %p", ttable[i]->retval);
      printf(" }\n");
    }
  }
}

void start(mypthread_t *t) {
  t->retval = t->fn(t->arg);
  t->state = TERMINATED;
  free(t->ctx.uc_stack.ss_sp);
  mypthread_yield();
}
