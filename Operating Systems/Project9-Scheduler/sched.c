/*Benjamin Schenker
 * Dec 9, 2013
 * PS 9
 * sched.c
 * ECE357*/
 
#include "sched.h"
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))


struct sched_proc *current, *next;//current always points to the sched_proc of 
									//the currently running task
struct sched_waitq q;//q has an element, procs which is an array of all tasks, 
						//indexed by pid. q.procs[0] is always NULL, and 0 is 
						//the parent of the init process whose pid is 1
						
unsigned int numticks=0;//global that contains total time of system since 
							//sched_init was called
sigset_t crit_mask;//will be used to mask and unmask SIGVTALRM

void sched_init(void (*init_fn)())//create initial task with pid of 1 and start execution
{
	//set up mask
	sigemptyset(&crit_mask);
	sigaddset(&crit_mask,SIGVTALRM);
	sigprocmask(SIG_BLOCK,&crit_mask,NULL);
	//set up signal handlers and interval timer
	signal(SIGVTALRM,sched_tick);
	signal(SIGABRT,sched_ps);
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 100000; //100msec
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec = 100000; //100msec
	setitimer(ITIMER_VIRTUAL,&timer,NULL);
	
	//create stack and initialize sched_proc
	void *init_sp;
	if((init_sp=mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0))==MAP_FAILED)
	{
		perror("mmap failed");
		exit(1);
	}
	current= (struct sched_proc*)malloc(sizeof(struct sched_proc));
	current->task_state = SCHED_RUNNING;
	current->niceval=0;
	current->quantum_val=20;//quantum_val is the number of ticks that each
								//process will be allowed to use, uninterrupted
								//it is a function of the niceval (see that function)
								//such that nicer processes get smaller uninterrupted
								//blocks of time, and vice versa
								
	current->act=0;//accumulated time for this process
	current->stackaddr = init_sp;
	current->pid=1;
	current->parent=0;//pid of parent, init's parent is 0
	current->dynamic_p = 0;//dynamic_p is the dynamic priority of the process
							//this number will be incremented for a process when
							//it has accumulated processor time.  The scheduling 
							//algorithm will schedule the process with the lowest
							//dynamic_p.  this also isa function of the niceval 
							//(see that function) such that nicer processes' dynamic
							//priority will be incremented faster than less nice ones
							
	current->time_slot=current->quantum_val;//time_slot contains the number of ticks 
											//left before the scheduler will check 
											//to find a process more deserving of 
											//cpu time
											
	current->exit=0;//if this process exits before its parent, it stores its exit 
					//code here
	current->child_exit=0;//if the parent called sched_wait before its child called
							//sched_exit, the child will place its exit code here
							//before removing itself
							
	q.procs[1]=current;
	
	//transfer execution to init_fn
		//because the stack starts at the highest part of the stack, the stack/base 
		//pointer is set to its highest memory address
	struct savectx ctx;
	ctx.regs[JB_PC]=init_fn;
	ctx.regs[JB_BP]=init_sp+STACK_SIZE;
	ctx.regs[JB_SP]=init_sp+STACK_SIZE;
	sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
	restorectx(&ctx,1);
}

int sched_fork()
{
	sigprocmask(SIG_BLOCK,&crit_mask,NULL);
	
	int i=1;
	while(1)//find valid unused pid
	{
		if (i==256)
		{
			fprintf(stderr,"too many processes!\n");
			return -1;
		}
		if (q.procs[i]==0)
		{
			break;
		}
		i++;
	}
	//set up child process
	next = (struct sched_proc*)malloc(sizeof(struct sched_proc));
	//*next=*current;
	next->pid=i;
	next->parent=current->pid;
	next->task_state=SCHED_READY;
	next->niceval=0;
	next->quantum_val=20;
	next->act=0;
	next->dynamic_p = 0;
	next->time_slot=next->quantum_val;
	next->exit=0;
	next->child_exit=0;
	void *new_sp;
	if((new_sp=mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0))==MAP_FAILED)
	{
		perror("mmap failed");
		return -1;
	}
	memcpy(new_sp,current->stackaddr,STACK_SIZE);
	next->stackaddr=new_sp;
	q.procs[i]=next;
	adjstack(new_sp,new_sp+STACK_SIZE,new_sp-current->stackaddr);
	
	//if in parent change childs stack/base pointers to point to the childs
	//stack.  Also return pid of child
	if (savectx(&(next->ctx))==0)
	{									
		next->ctx.regs[JB_BP]+=new_sp-current->stackaddr;
		next->ctx.regs[JB_SP]+=new_sp-current->stackaddr;
		sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
		return next->pid;
	}
	else //in child
	{
		sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
		return 0;
	}
}

void sched_exit(int code)
{
	sigprocmask(SIG_BLOCK,&crit_mask,NULL);
	current->task_state=SCHED_ZOMBIE;
	//if parent already called wait
	if ((q.procs[q.procs[current->pid]->parent])
				&&q.procs[q.procs[current->pid]->parent]->task_state
				==SCHED_SLEEPING)
	{
		q.procs[q.procs[current->pid]->parent]->child_exit=code;
		q.procs[q.procs[current->pid]->parent]->task_state=SCHED_READY;
		q.procs[current->pid]=0;//removing from currently running processes
	}
	else
	{
		current->exit=code;
	}
	
	sched_switch();
}
unsigned int sched_zombie()
//returns zero if no children, SCHED_NPROC if children but none zombies, 
//and pid of child if zombie child exists
{
	unsigned int i=0;
	unsigned int f_ind=0;
	for (i = 1; i < SCHED_NPROC; i++)
	{
		if (q.procs[i]
					&&(q.procs[i]->parent ==current->pid))
		{
			if ((q.procs[i]->task_state==SCHED_ZOMBIE))
			{
				f_ind=i;
				break;
			}
			f_ind=SCHED_NPROC;
		}
	}
	return f_ind;
}
int sched_wait(int *exit_code)
{
	sigprocmask(SIG_BLOCK,&crit_mask,NULL);
	unsigned int z_ind=sched_zombie();
	switch(z_ind)
	{
		case 0://no children
			return -1;
			break;
		case SCHED_NPROC://children but not zombies yet
			current->task_state=SCHED_SLEEPING;
			sched_switch();
			*exit_code=current->child_exit;
			current->child_exit=0;
			break;
		default://zombie children
			*exit_code=q.procs[z_ind]->exit;
			q.procs[z_ind]=0;//removing from currently running processes
			break;
	}
	
	sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
	return 0;
}

void sched_nice(int niceval)
{ 
/*
fairly simple scheduling algorithm, where niceval (static priority)
affects the amount of uninterrupted time a process is allowed and
also it affects how much the dynamic priority is increased.

nice processes will have a small amount of uninterrupted time
and also their dynamic priority will be increase by a large value
for every tick of processor time they use.

the process with the lowest dynamic priority that is runnable will be 
scheduled next. see sched_tick for exact increment
*/
	sigprocmask(SIG_BLOCK,&crit_mask,NULL);
	niceval = MAX(-20,niceval);
    niceval = MIN(19,niceval);
    current->niceval=niceval;
    current->quantum_val=20-niceval;
    current->dynamic_p=(21+niceval)*current->act;
    current->time_slot=current->quantum_val;
	sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
}

unsigned int sched_getpid()
{
	return current->pid;
}

unsigned int sched_getppid()
{
	return current->parent;
}

unsigned int sched_gettick()
{
	return numticks;
}

void sched_ps()
{
	printf("pid\tppid\tstate\tstack\t\twq\t\tspr\tdpr\ttime\n");
	int i;
	for (i = 1; i < SCHED_NPROC; i++)
	{
		if (q.procs[i]!=0)
		{
			printf("%d\t%d\t%d\t%p\t%p\t%d\t%d\t%d\n",q.procs[i]->pid
											,q.procs[i]->parent
											,q.procs[i]->task_state
											,q.procs[i]->stackaddr
											//not really address of waitq
											//because there is no waitq
											,q.procs[i] 
											,q.procs[i]->niceval
											,q.procs[i]->dynamic_p
											,q.procs[i]->act);
		}
	}
}

unsigned int sched_findlowest()
//returns 0 if no procs are runnable, otherwise returns runnable process 
//with lowest (best) dynamic priority
{
	unsigned long long best=ULLONG_MAX;
	unsigned int i=0;
	unsigned int f_ind=0;
	for (i = 1; i < SCHED_NPROC; i++)
	{
		if (q.procs[i]
					&&(q.procs[i]->task_state==SCHED_READY)
					&&(q.procs[i]->dynamic_p < best))
		{
			best=q.procs[i]->dynamic_p;
			f_ind=i;
		}
	}
	return f_ind;
	
}

void sched_switch()
{
	sched_ps();
	if(current->task_state==SCHED_RUNNING)//don't mess with sleeping or zombie tasks
	{
		current->task_state=SCHED_READY;
		current->time_slot=current->quantum_val;
	}
	//get next process
	next=q.procs[sched_findlowest()];
	if (next==NULL)
	{
		fprintf(stderr,"all procs ended\n");
		sched_ps();
		exit(0);
	}
	//switch contexts
	if (savectx(&(current->ctx))==0)//if in current
	{									
		current=next;
		current->task_state=SCHED_RUNNING;
		restorectx(&(current->ctx),1);
	}
	
	sigprocmask(SIG_UNBLOCK,&crit_mask,NULL);
}

void sched_tick() //timer signal handler
{
	numticks++;
	current->act++;
	current->time_slot--;
	//update for dynamic priority here
	current->dynamic_p=(21+current->niceval)*current->act;
	if (!current->time_slot)
	{
		sched_switch();
	}
}
