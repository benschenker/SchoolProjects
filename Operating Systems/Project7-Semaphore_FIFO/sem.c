/*Benjamin Schenker
 * Nov 25, 2013
 * PS 7
 * sem.c
 * ECE357*/
 
 #include "sem.h"

void sighand(int sig)
{
	//fprintf(stderr,"waking up\n");
}

void sem_init(struct sem *s, int count)
{
	s->count=count;
	s->lock=0;
	//memset(s->q, 0, sizeof(s->q)); //the source of many troubles
}

int sem_try(struct sem *s)
{
	while(tas(&(s->lock))!=0) 
	{}
	if (s->count==0)//would block
	{
		s->lock=0;
		return 0;
	}
	s->lock=0;
	return 1;
}

void sem_wait(struct sem *s)
{
	pid_t pid;
	int i;
	while(tas(&(s->lock))!=0) 
	{}
	while (s->count==0)//would block
	{
		pid=getpid();
		for (i = 0; i < N_PROC; i ++)
		{
			if (s->q[i]==0)
			{
				break; //break out when first empty entry is found.  
					   // This will cause problems if all entries are full
			}
		}
		if (i>=64)
		{
			fprintf(stderr,"HELP!!!\n");
			exit(1);
		}
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask,SIGUSR1);
		signal(SIGUSR1,sighand);
		s->q[i]=pid;
		s->lock=0;
		sigsuspend(&mask);
		while(tas(&(s->lock))!=0) 
		{}
	}
	s->count--;
	s->lock=0;
}

void sem_inc(struct sem *s)
{
	while(tas(&(s->lock))!=0) 
	{}
	s->count++;
	int i;
	pid_t pid;
	for (i = 0; i < N_PROC; i++)
	{
		pid=s->q[i];
		if (pid>0)
		{
			s->q[i]=0;
			kill(pid,SIGUSR1);
			//break;
		}
	}
	s->lock=0;
}
