#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Constants in the code
#define FIRST_ROOM_CAPACITY 5
#define FIRST_ROOM_SERVICE_RATE 1

#define SECOND_ROOM_CAPACITY 2
#define SECOND_ROOM_SERVICE_RATE 2
#define WAITING_ROOM_CAPACITY 2

#define NUM_PEOPLE_ARRIVING 10

// capture the of customers that leave early
int num_left_customers = 0;
// TODO: Define other state of the world variables here
int room1_in_service = 0;
int room2_in_waiting = 0;
int room2_in_service = 0;

// TODO: Define your mutex locks and condition variables:
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t room1_service_freed = PTHREAD_COND_INITIALIZER;
pthread_cond_t room2_service_freed = PTHREAD_COND_INITIALIZER;

void *customer(void *arg)
{
  long int tid = (long int)arg;

  printf("[Customer %ld] Just arrived at first room...\n", tid);

  // Enter the first room.
  pthread_mutex_lock(&mutex);
  while (room1_in_service >= FIRST_ROOM_CAPACITY) {
    pthread_cond_wait(&room1_service_freed, &mutex);
  }
  room1_in_service += 1;
  printf("[Customer %ld] Entered first room...\n", tid);
  pthread_mutex_unlock(&mutex);
  sleep(FIRST_ROOM_SERVICE_RATE);
  pthread_mutex_lock(&mutex);
  room1_in_service -= 1;
  printf("[Customer %ld] Left first room...\n", tid);
  pthread_cond_signal(&room1_service_freed);
  pthread_mutex_unlock(&mutex);

  // You might want to check if you need to enter the waiting room here or leave
  // here...
  pthread_mutex_lock(&mutex);
  if (room2_in_service >= SECOND_ROOM_CAPACITY) {
    if (room2_in_waiting >= WAITING_ROOM_CAPACITY) {
      num_left_customers += 1;
      printf("[Customer %ld] Left unhapy because waiting room is full...\n", tid);
      pthread_mutex_unlock(&mutex);
      return 0;
    } else {
      // In case the customer wants to enter the waiting room, you might want to
      // print the following:
      room2_in_waiting += 1;
      printf("[Customer %ld] Joined the waiting room for second room...\n", tid);
      pthread_cond_wait(&room2_service_freed, &mutex);
      room2_in_waiting -= 1;
    }
  }

  // Enter the second room
  room2_in_service += 1;
  printf("[Customer %ld] Entered second room...\n", tid);
  pthread_mutex_unlock(&mutex);
  sleep(SECOND_ROOM_SERVICE_RATE);
  room2_in_service -= 1;
  pthread_mutex_lock(&mutex);
  printf("[Customer %ld] Left second room...\n", tid);
  pthread_cond_signal(&room2_service_freed);
  pthread_mutex_unlock(&mutex);

  // Done, time to leave...
  return 0;
}

int
main(int argc, char **argv)
{
  int i;
  long int tids[NUM_PEOPLE_ARRIVING];
  pthread_t threads[NUM_PEOPLE_ARRIVING];
  srand(time(0));

  for(i = 0; i < NUM_PEOPLE_ARRIVING; i++) {
    tids[i] = i + 1;
    pthread_create(&threads[i], 0, customer, (void*)tids[i]);
    if(!(i % 2))
      sleep(rand() % 2);
  }

  for(i = 0; i < NUM_PEOPLE_ARRIVING; i++) {
    pthread_join(threads[i], 0);
  }

  printf("Everything finished: %d customers left unhappy...\n",
         num_left_customers);
  exit(EXIT_SUCCESS);
}
