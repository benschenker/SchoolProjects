/*Benjamin Schenker
 * Nov 25, 2013
 * PS 7
 * main.c
 * ECE357*/
 #include "fifo.h"
 int main(int argc, char** argv)
 {
 	if (argc<3)
 	{
 		fprintf(stderr,"usage: ./main.exe numprocs nums\n");
 		exit(1);
 	}
 	int procs = atoi(argv[1]); //number includes reader
 	if (procs>N_PROC)
 	{
 		fprintf(stderr,"max procs is %i, min is 2 \n",N_PROC); //assume 64
 		exit(1);
 	}
 	int numelems=atoi(argv[2]);
 	struct fifo *f=(struct fifo*)mmap(NULL,sizeof(struct fifo),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,-1,0);
 	fifo_init(f);
 	unsigned long r,procnum;
 	unsigned long tot=0;
 	unsigned long shouldbe=0;
 	int i,j;
 	for (i = 0; i < procs-1; i++)
 	{
 		switch(fork())
		{
			case -1:
				perror("fork failed");exit(1);
				break;
			case 0:
				for (j = 0; j < numelems; j++)
				{
					fifo_wr(f,i|(j<<6));//6 least significant bits are process number i, 
										//other bits are shifted versions of the payload
				}
				fprintf(stderr,"procnum %i has finished writing\n",i);
				exit(0);
				break;
		}
 	}
	for (i = 0; i < (procs-1); i++)
	{
		for (j = 0; j < numelems; j++)
		{
			r=fifo_rd(f);
			procnum=r&((long)63); //masking off 6 least significant bits
			r=r>>6;
			tot+=r;
			shouldbe+=j;
		}
	}
	if (tot==shouldbe)
	{
		fprintf(stderr,"Received %i messages each from %i procs.  All messages accounted for, YAY\n",numelems,procs-1);
	}
	else
	{
		fprintf(stderr," LOSE, tot=%i,shouldbe=%i\n",tot,shouldbe);
	}
 	return 0;
 }
