#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// constants to control number of cars and threads
#define NUM_EW_CARS 15
#define NUM_WE_CARS 15
#define NUM_AMBULANCE 7
#define TOTAL_THREADS NUM_EW_CARS + NUM_WE_CARS + NUM_AMBULANCE

// constants to define ambulance direction
#define EW_DIRECTION 1
#define WE_DIRECTION 2

// car speeds
#define EW_SPEED 3
#define WE_SPEED 2

// tunnel sizes
#define EW_CAPACITY 3
#define WE_CAPACITY 1

// state variables
int ew_cars_in_tunnel = 0;
int we_cars_in_tunnel = 0;
int ambulances_in_tunnel = 0;
int ambulances_waiting = 0;

// concurrency means
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vehicle_left_tunnel = PTHREAD_COND_INITIALIZER;

void *ewcar(void *arg)
{
  int tid = *(int*)arg;

  pthread_mutex_lock(&mutex);
  while (ew_cars_in_tunnel >= EW_CAPACITY || ambulances_in_tunnel > 0 || ambulances_waiting > 0) {
    pthread_cond_wait(&vehicle_left_tunnel, &mutex);
  }
  ew_cars_in_tunnel += 1;
  printf("Car (%d) entered tunnel in EW direction...\n", tid);
  pthread_mutex_unlock(&mutex);

  sleep(EW_SPEED);

  pthread_mutex_lock(&mutex);
  ew_cars_in_tunnel -= 1;
  printf("Car (%d) exited tunnel in EW direction...\n", tid);
  pthread_cond_broadcast(&vehicle_left_tunnel);
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *wecar(void *arg)
{
  int tid = *(int *)arg;

  pthread_mutex_lock(&mutex);
  while (we_cars_in_tunnel >= WE_CAPACITY || ambulances_in_tunnel > 0 || ambulances_waiting > 0) {
    pthread_cond_wait(&vehicle_left_tunnel, &mutex);
  }
  we_cars_in_tunnel += 1;
  printf("Car (%d) entered tunnel in WE direction...\n", tid);
  pthread_mutex_unlock(&mutex);

  sleep(WE_SPEED);

  pthread_mutex_lock(&mutex);
  we_cars_in_tunnel -= 1;
  printf("Car (%d) exited tunnel in WE direction...\n", tid);
  pthread_cond_broadcast(&vehicle_left_tunnel);
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *ambulance(void *arg)
{
  int tid = *(int*)arg;
  int direction = EW_DIRECTION;

  pthread_mutex_lock(&mutex);
  ambulances_waiting += 1;
  while (ew_cars_in_tunnel + we_cars_in_tunnel + ambulances_in_tunnel >= EW_CAPACITY + WE_CAPACITY) {
    pthread_cond_wait(&vehicle_left_tunnel, &mutex);
  }
  ambulances_waiting -= 1;
  ambulances_in_tunnel += 1;
  if(direction == EW_DIRECTION) {
    printf("Ambulance %d entered the tunnel in EW direction\n", tid);
  } else if (direction == WE_DIRECTION) {
    printf("Ambulance %d entered the tunnel in WE direction\n", tid);
  }
  pthread_mutex_unlock(&mutex);

  sleep(1); // ambulance is the fastest

  pthread_mutex_lock(&mutex);
  ambulances_in_tunnel -= 1;
  if(direction == EW_DIRECTION) {
    printf("Ambulance %d exited the tunnel in EW direction\n", tid);
  } else if(direction == WE_DIRECTION) {
    printf("Ambulance %d exited the tunnel in WE direction\n", tid);
  }
  pthread_cond_broadcast(&vehicle_left_tunnel);
  pthread_mutex_unlock(&mutex);

  return 0;
}


int
main(int argc, char **argv)
{
  pthread_t threads[NUM_EW_CARS + NUM_WE_CARS + NUM_AMBULANCE];
  int tids[NUM_EW_CARS + NUM_WE_CARS + NUM_AMBULANCE];
  int i;
  srand(time(0));

  for(i = 0; i < NUM_EW_CARS; i++) {
    tids[i] = i + 1;
    pthread_create(&threads[i], 0, ewcar, &tids[i]);
  }

  for(i = NUM_EW_CARS; i < NUM_WE_CARS + NUM_EW_CARS; i++) {
    tids[i] = i + 1;
    pthread_create(&threads[i], 0, wecar, &tids[i]);
  }

  for(i = NUM_EW_CARS + NUM_WE_CARS; i < TOTAL_THREADS; i++) {
    tids[i] = i + 1;
    pthread_create(&threads[i], 0, ambulance, &tids[i]);
    // make the ambulances arrive at random times
    sleep(rand() % 10);
  }

  for(i = 0; i < TOTAL_THREADS; i++) {
    pthread_join(threads[i], 0);
  }

  printf("Everyting finished...\n");
  exit(EXIT_SUCCESS);
}
