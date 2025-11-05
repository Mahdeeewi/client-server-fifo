#ifndef SERV_CLI_FIFO_H
#define SERV_CLI_FIFO_H

/* System headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>

/* Constants and macros */
#define NMAX 100
#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"

/* Data structures */

/* Question structure sent from client to server */
struct question {
    int client_id;      /* Client PID */
    int n;              /* Number of random numbers requested (1 to NMAX) */
};

/* Response structure sent from server to client */
struct reponse {
    int client_id;      /* Client PID */
    int server_id;      /* Server PID - ADDED THIS */
    int numbers[NMAX];  /* Array of random numbers */
};

#endif /* SERV_CLI_FIFO_H */
