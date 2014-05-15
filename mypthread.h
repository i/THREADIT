#ifndef MYPTHREAD_H
#define MYPTHREAD_H
#include <ucontext.h>
#include <signal.h>

#define MAX_THREADS 1024
#define STACK_SZ 0x4000

int tableup; // Whether or not thread table has been initialized
int pos;     // Current position in thread table
enum status { UNUSED, BLOCKED, WAITING, RUNNABLE, TERMINATED };

/*
 * pthread type
 */
typedef struct mypthread {
  void*(*fn)(void*);
  void *arg;
  enum status state;
  ucontext_t ctx;
  void *retval;
  int id;
  int join_id;
} mypthread_t;

mypthread_t main_thread;
mypthread_t *ttable[MAX_THREADS];

/*
 * creates a new thread
 */
int mypthread_create(mypthread_t *thread,
    void *(*start_routine) (void *), void *arg);

/*
 * terminates the calling thread
 */
void mypthread_exit(void *retval);

/*
 * yield the processor causing the calling thread to relinquish the CPU
 * returns 0 on success; returns error number on failure
 */
int mypthread_yield(void);

/*
 * join with a terminated thread.
 */
int mypthread_join(mypthread_t thread, void **retval);

/*
 * set up ttable
 */
void ttable_init();
void scheduler();
mypthread_t *next_thread(void);
void start_timer(void);
void print_thread_table(void);
void start(mypthread_t*);
void handler(int);

#endif
