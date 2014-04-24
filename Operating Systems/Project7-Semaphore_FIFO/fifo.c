/*Benjamin Schenker
 * Nov 25, 2013
 * PS 7
 * fifo.c
 * ECE357*/
 
 #include "fifo.h"

void fifo_init(struct fifo *f)
{
	f->beg=0;//next place to read from
	f->end=0;//next place to write to
	sem_init(&f->access,1);
	sem_init(&f->write,MYFIFO_BUFSIZ);
	sem_init(&f->read,0);
}

void fifo_wr(struct fifo *f, unsigned long d)
{
	sem_wait(&f->write);
	sem_wait(&f->access);
	f->addr[f->end%MYFIFO_BUFSIZ]=d;
	f->end++;
	sem_inc(&f->access);
	sem_inc(&f->read);
}

unsigned long fifo_rd(struct fifo *f)
{
	sem_wait(&f->read);
	sem_wait(&f->access);
	unsigned long l=f->addr[f->beg%MYFIFO_BUFSIZ];
	f->beg++;
	sem_inc(&f->access);
	sem_inc(&f->write);
	return l;
}
