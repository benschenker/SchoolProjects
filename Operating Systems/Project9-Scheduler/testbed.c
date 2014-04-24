/*Benjamin Schenker
 * Dec 9, 2013
 * PS 9
 * testbed.c
 * ECE357*/
 
 #include "sched.h"

/*
This initializes the scheduler with init_fn, which then forks twice (one child becomes 
nice and the other is inconsiderate) and continues to do its own thing, then waits for 
both its children and exits. 

Each process counts to a very high number and prints some information ten times 
throughout the process.

This has been designed such that
pid 1 will fork twice then start counting
then pid 2 will do its whole job (uninterrupted)
then pid 3 will do a very small amount before
pid 1 interrupts it and finishes
pid 3 will then take its time to finish (with many interruptions)

each pid does the same task so it should take the same amount of time per task.
*/

void child1()
{
 	fprintf(stderr,"HIIII child here! my pid is %d and my parent is %d\n",sched_getpid(),sched_getppid());
 	long long i;
 	for (i = 0; i < 1000000000; i++)
 	{
 		
 		if (i%100000000==0)
 		{
 			fprintf(stderr,"pid=%d, i=%lld, ticks=%d\n",sched_getpid(),i,sched_gettick());
 		}
 		
 	}
 	sched_exit(1);
 }
void child2()
{
 	fprintf(stderr,"HIIII child here! my pid is %d and my parent is %d\n",sched_getpid(),sched_getppid());
 	long long i;
 	for (i = 0; i < 1000000000; i++)
 	{
 		
 		if (i%100000000==0)
 		{
 			fprintf(stderr,"pid=%d, i=%lld, ticks=%d\n",sched_getpid(),i,sched_gettick());
 		}
 		
 	}
 	sched_exit(2);
 }
 
void parent()
{
 	fprintf(stderr,"parent hello world my pid is %d and my parent is %d\n",sched_getpid(),sched_getppid());
 	long long i;
 	for (i = 0; i < 1000000000; i++)
 	{
 		
 		if (i%100000000==0)
 		{
 			fprintf(stderr,"pid=%d, i=%lld, ticks=%d\n",sched_getpid(),i,sched_gettick());
 		}
 		
 	}
 	int ret;
 	sched_wait(&ret);
 	printf("one child returned %d\n",ret);
 	sched_wait(&ret);
 	printf("another child returned %d\n",ret);
 	sched_exit(0);
 }
 
void init_fn()
{
	int j;
	for (j = 0; j < 2; j++)
	{
	 	switch(sched_fork())
	 	{
	 		case -1:
	 			fprintf(stderr, "fork error!\n");
	 			exit(1);
	 		case 0:
	 			if (!j)
	 			{
	 				sched_nice(-20);
	 				child1();
	 			}
	 			else
	 			{
	 				sched_nice(20);
	 				child2();
	 			}
	 			
	 			exit(0);
	 	}
	}
	//sched_nice(5);
 	parent();
 	exit(0);
 }
 
 int main(int argc, char** argv)
 {
 	sched_init(init_fn);
 	return 0;
 }
 

