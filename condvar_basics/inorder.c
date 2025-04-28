#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t thread_finished;
int last_thread = 0;

void *thread(void *arg)
{
  int *num = (int *)arg;
  printf("%d wants to enter the critical section\n", *num);

  pthread_mutex_lock(&mutex);
  while (last_thread != *num - 1) {
    pthread_cond_wait(&thread_finished, &mutex);
  }
  last_thread += 1;
  pthread_cond_signal(&thread_finished);
  pthread_mutex_unlock(&mutex);

  printf("%d is finished with the critical section\n", *num);

  return NULL;
}

int
main(int argc, char **argv)
{
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&thread_finished, 0);

  pthread_t threads[4];
  int i;
  int nums[] = {2, 1, 4, 3};

  for(i = 0; i < 4; ++i) {
    pthread_create(&threads[i], NULL, thread, &nums[i]);

    if(i == 2)
      sleep(3);
  }

  for(i = 0; i < 4; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Everything finished\n");

  return 0;
}
