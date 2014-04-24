/*Benjamin Schenker
 * Dec 9, 2013
 * PS 9
 * sched.h
 * ECE357*/
  
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <signal.h>
#include <limits.h>
#include "savectx64.h"
#define STACK_SIZE 65536
#define SCHED_NPROC 256
#define SCHED_READY 0
#define SCHED_RUNNING 1
#define SCHED_SLEEPING 2
#define SCHED_ZOMBIE 3



struct sched_proc //change vals
{
	int task_state;
	int niceval;
	int act; //accumulated cputime
	int quantum_val;
	int time_slot;
	void* stackaddr;
	unsigned int pid;
	int child_exit;
	int exit;
	unsigned int parent; //pid of parent
	unsigned long long dynamic_p;
	struct savectx ctx;
	
};

struct sched_waitq //change vals
{
	struct sched_proc* procs[STACK_SIZE];
};

void sched_init(void (*init_fn)());
int sched_fork();
void sched_exit(int code);
int sched_wait(int *exit_code);
void sched_nice(int niceval);
unsigned int sched_getpid();
unsigned int sched_getppid();
unsigned int sched_gettick();
void sched_ps();
void sched_switch();
void sched_tick();

