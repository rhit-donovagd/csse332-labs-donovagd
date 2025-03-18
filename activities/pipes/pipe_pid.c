#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * pipe_pid.c
 *
 * Task:
 * =====
 * In this exercise, we would like to child to send its process id back to its
 * parent. Yes we can get it from the return value of fork, but we'd like to
 * have a backup way for getting it as well.
 *
 * HINT 1:
 * =======
 * You might find the functions sprintf, snprintf, and atoi very helpful.
 *
 * HINT 2:
 * =======
 *  Alternatively, remember that everything is just a bunch of bytes.  How many
 *  bytes is an integer?
 */

int main(int argc, char **argv) {
  pid_t pid;

  int fd[2];
  pipe(fd);

  pid = fork();
  if(pid == 0) {
    // child, I am the writer
    close(fd[0]);

    char data[512];
    sprintf(data, "%d", getpid());
    write(fd[1], data, strlen(data));

    close(fd[1]);
    exit(0);
  }

  // parent, I am the reader
  close(fd[1]);

  char read_data[512];
  int recv_bytes = read(fd[0], read_data, 512);
  read_data[recv_bytes] = '\0';

  printf("expected pid:  %d\n", pid);
  printf("pid from pipe: %s\n", read_data);

  close(fd[0]);
  exit(0);
}

