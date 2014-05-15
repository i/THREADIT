#include <stdio.h>
#include "mymutex.h"

int main(void) {
  uint a = 0;
  xchg(&a, 1);
  printf("%d\n", a);

  return 0;
}
