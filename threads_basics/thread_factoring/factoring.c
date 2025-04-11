/* Copyright 2016 Rose-Hulman Institute of Technology

   Here is some code that factors in a super dumb way.  We won't be
   attempting to improve the algorithm in this case (though that would be
   the correct thing to do).

   Modify the code so that it starts the specified number of threads and
   splits the computation among them.  You can be sure the max allowed 
   number of threads is 50.  Be sure your threads actually run in parallel.

   Your threads should each just print the factors they find, they don't
   need to communicate the factors to the original thread.

   ALSO - be sure to compile this code with -pthread or just used the 
   Makefile provided.

*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>

struct factor_args {
  int thread_index;
  int target;
  int inc;
};

void* factor(void* args_void) {
    struct factor_args* args = (struct factor_args*)args_void;

    for (int i = 2 + args->thread_index; i <= args->target / 2; i += args->inc) {
	/* You'll want to keep this testing line in.  Otherwise it goes so
	   fast it can be hard to detect your code is running in
	   parallel. Also test with a large number (i.e. > 3000) */
	printf("thread %d: testing %d\n", args->thread_index, i);
	if (args->target % i == 0) {
	  printf("%d is a factor\n", i);
	}
    }

    return NULL;
}

int main(void) {
  /* you can ignore the linter warning about this */
  unsigned long long target, i;
  int numThreads;
  printf("Give a number to factor.\n");
  scanf("%llu", &target);

  printf("How man threads should I create?\n");
  scanf("%d", &numThreads);
  if (numThreads > 50 || numThreads < 1) {
    printf("Bad number of threads!\n");
    return 0;
  }

  pthread_t threads[numThreads];
  struct factor_args* thread_args[numThreads];
  for (i = 0; i < numThreads; i++) {
    struct factor_args* args = malloc(sizeof(struct factor_args));
    args->thread_index = i;
    args->target = target;
    args->inc = numThreads;
    thread_args[i] = args;

    pthread_create(&threads[i], NULL, factor, (void*)(thread_args[i]));
  }

  for (i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}

