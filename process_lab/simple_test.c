#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define simple_assert(message, test)                                           \
  do {                                                                         \
    if(!(test))                                                                \
      return message;                                                          \
  } while(0)
#define TEST_PASSED NULL
#define DATA_SIZE 100
#define INITIAL_VALUE 77
#define MAX_TESTS 10

/* test_funcs is an array of function pointers that store all of the test we
 * would like to run. */
char *(*test_funcs[MAX_TESTS])();

/* total number of tests registered */
static int num_tests = 0;

/* shared data that all the tests will use. */
static int data[DATA_SIZE][DATA_SIZE];

/**
 * Add a test to our list of test functions that we would like to run.
 *
 * @param test_func A pointer to the function that we would like to run.
 */
void add_test(char *(*test_func)()) {
  if(num_tests == MAX_TESTS) {
    printf("exceeded max possible tests");
    exit(1);
  }
  test_funcs[num_tests] = test_func;
  num_tests++;
}

void signal_handler(int signum) {
    switch (signum) {
	case SIGALRM:
	    exit(2);
	    break;
    }
}

/**
 * This setup function should run before each test
 */
void setup(void) {
  printf("starting setup\n");
  fflush(stdout);
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = INITIAL_VALUE;
    }
  }

  struct sigaction sigalrm_action;
  sigalrm_action.sa_handler = signal_handler;
  sigemptyset(&sigalrm_action.sa_mask);
  sigalrm_action.sa_flags = SA_RESTART;

  sigaction(SIGALRM, &sigalrm_action, NULL);

  /* imagine this function does a lot of other complicated setup that takes a
   * long time. */
  usleep(3000000);
}

/**
 * Run all the test in the test suite.
 */
void run_all_tests() {
    setup();

    int children[num_tests];
    int pipes[num_tests];

    for (int i = 0; i < num_tests; i++) {
	int fd[2];
	pipe(fd);

	children[i] = fork();
	if (children[i] == 0) {
	    close(fd[0]);

	    alarm(3);

	    char* test_result = test_funcs[i]();

	    if (test_result == TEST_PASSED) {
		close(fd[1]);
		exit(0);
	    } else {
		write(fd[1], test_result, strlen(test_result));
		close(fd[1]);
		exit(1);
	    }
	} else {
	    close(fd[1]);
	    pipes[i] = fd[0];
	}
    }

    char err_msg[512];
    int bytes_read;
    int wstatus;
    for (int i = 0; i < num_tests; i++) {
	waitpid(children[i], &wstatus, 0);

	if (WIFEXITED(wstatus)) {
	    int exit_status = WEXITSTATUS(wstatus);
	    switch (exit_status) {
		case 0:
		    printf("Test Passed\n");
		    break;
		case 1:
		    bytes_read = read(pipes[i], err_msg, 512);
		    err_msg[bytes_read] = '\0';

		    printf("Test Failed: %s\n", err_msg);
		    break;
		case 2:
		    printf("Test Timed Out\n");
		    break;
		default:
		    printf("Test exited with unknown status `%d`\n", exit_status);
	    }
	} else {
	    printf("Test Crashed\n");
	}

	close(pipes[i]);
    }


}

char *test1() {
  printf("starting test 1\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 1;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not set properly", data[i][j] == 1);
    }
  }
  printf("ending test 1\n");
  return TEST_PASSED;
}

char *test2() {
  printf("starting test 2\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 2;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not set properly", data[i][j] == 2);
    }
  }

  printf("ending test 2\n");
  return TEST_PASSED;
}

char *test3() {
  printf("starting test 3\n");

  simple_assert("test 3 always fails", 1 == 2);

  printf("ending test 3\n");
  return TEST_PASSED;
}

char *test4() {
  printf("starting test 4\n");
  fflush(stdout);

  int *val = NULL;
  printf("data at val is %d", *val);

  printf("ending test 4\n");
  return TEST_PASSED;
}

char *test5() {
  printf("starting test 5\n");
  fflush(stdout);

  
  while(1)
    ;

  printf("ending test 5\n");
  return TEST_PASSED;
}

int main(int argc, char **argv) {
  add_test(test1);
  add_test(test2);
  add_test(test3);
  add_test(test4);
  add_test(test5);
  run_all_tests();
}
