/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
   */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

void setsighandler(int signum, void (*handler)(int)) {
  struct sigaction act;

  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;
  sigaction(signum, &act, NULL);
}

void handle_sigchld(int ignored) {
  wait(NULL);
}

void run_command(char** command, int background) {
  if (background) {
    if (fork() == 0) {
      if (fork() == 0) {
        execvp(command[0], command);
        perror("exec failed");
        exit(1);
      } else {
        wait(NULL);
	printf("Background command finished");
        exit(EXIT_SUCCESS);
      }
    }
  } else {
    if (fork() == 0) {
      execvp(command[0], command);
      perror("exec failed");
      exit(1);
    } else {
      wait(NULL);
    }
  }
}

int main() {
  setsighandler(SIGCHLD, handle_sigchld);

  char command[82];
  char *parsed_command[3];
  parsed_command[2] = NULL;
  //takes at most two input arguments
  // infinite loop but ^C quits
  while (1) {
    printf("SHELL%% ");
    fgets(command, 82, stdin);
    command[strlen(command) - 1] = '\0';//remove the \n

    int len_1;
    for(len_1 = 0;command[len_1] != '\0';len_1++){
      if(command[len_1] == ' ')
        break;
    }
    parsed_command[0] = command;
    if(len_1 == strlen(command)){
      printf("Command is '%s' with no arguments\n", parsed_command[0]);
      parsed_command[1] = NULL;
    } else {
      command[len_1] = '\0';
      parsed_command[1] = command + len_1 + 1;
      printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]);
    }

    // leave this here, do not change it
    if(!strcmp(parsed_command[0], "quit") ||
        !strcmp(parsed_command[0], "exit")) {
      exit(EXIT_SUCCESS);
    }

    long executions = 1;
    if (isdigit(parsed_command[0][0]) != 0) {
      executions = strtol(&parsed_command[0][0], NULL, 10);
      for (int i = 0; i < strlen(parsed_command[0]); i++) {
        parsed_command[0][i] = parsed_command[0][i + 1];
      }
      parsed_command[0][strlen(parsed_command[0])] = '\0';
    }

    int background = 0;
    if (strncmp(parsed_command[0], "BG", 2) == 0) {
      background = 1;
      for (int i = 0; i < strlen(parsed_command[0]) - 1; i++) {
        parsed_command[0][i] = parsed_command[0][i + 2];
      }
      parsed_command[0][strlen(parsed_command[0])] = '\0';
    }

    if (executions > 1) {
      printf("Will generate %ld commands %s\n", executions, parsed_command[0]);
    }

    for (int i = 0; i < executions; i++) {
      run_command(parsed_command, background);
    }
  }
}
