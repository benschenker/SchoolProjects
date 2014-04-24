/*Benjamin Schenker
 * Nov 25, 2013
 * PS 7
 * sem.h
 * ECE357*/
  
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#define N_PROC 64

struct sem
{
	int count;
	pid_t q[N_PROC]; //a "queue".  Really an array of pids
	volatile char lock;
};

int tas(volatile char *lock);

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);
