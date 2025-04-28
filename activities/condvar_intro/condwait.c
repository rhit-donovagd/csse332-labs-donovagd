#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

pthread_cond_t child_halfway;
pthread_mutex_t mutex;
int child_passed_halfway = 0;

void *
child(void *ignored) {
    /* modify this code to let the parent know the child is done */
    printf("Child\n");

    sleep(5);

    pthread_mutex_lock(&mutex);
    printf("signaling parent\n");
    child_passed_halfway = 1;
    pthread_cond_signal(&child_halfway);
    pthread_mutex_unlock(&mutex);

    sleep(5);

    printf("Child completed\n");

    return 0;
}

int
main(int argc, char **argv) {
	pthread_t thread;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&child_halfway, 0);

	pthread_create(&thread, 0, child, NULL);

	printf("Parent waiting for the child to halfway finish\n");

	pthread_mutex_lock(&mutex);
	while (child_passed_halfway) {
	    pthread_cond_wait(&child_halfway, &mutex);
	}
	pthread_mutex_unlock(&mutex);

	printf("Parent working with child\n");
	sleep(2);
	printf("Parent done\n");

	/* done */
	exit(0);
}
