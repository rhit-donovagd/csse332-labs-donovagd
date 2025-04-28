#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t thread_finished;
int threads_in_critical = 0;

void *thread(void *arg)
{
	char *letter = (char *)arg;
	printf("%c wants to enter the critical section\n", *letter);

	pthread_mutex_lock(&mutex);
	while (threads_in_critical >= 3) {
		pthread_cond_wait(&thread_finished, &mutex);
	}
	threads_in_critical += 1;
	pthread_mutex_unlock(&mutex);

	printf("%c is in the critical section\n", *letter);
	sleep(1);

	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&thread_finished);
	threads_in_critical -= 1;
	pthread_mutex_unlock(&mutex);
	printf("%c has left the critical section\n", *letter);

	return NULL;
}

int
main(int argc, char **argv)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&thread_finished, 0);

	pthread_t threads[8];
	int i;
	char *letters = "abcdefgh";

	for(i = 0; i < 8; ++i) {
		pthread_create(&threads[i], NULL, thread, &letters[i]);

		if(i == 4)
			sleep(4);
	}

	for(i = 0; i < 8; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("Everything finished...\n");

	return 0;
}
