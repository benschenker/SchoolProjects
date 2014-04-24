/*Benjamin Schenker
 * Nov 25, 2013
 * PS 7
 * fifo.h
 * ECE357*/
 
#include <sys/mman.h>
#include "sem.h"
#define MYFIFO_BUFSIZ 4096

struct fifo
{
	unsigned long addr[MYFIFO_BUFSIZ];
	unsigned int beg,end;//indices into addr
	struct sem access, write, read;
};

void fifo_init(struct fifo *f);

void fifo_wr(struct fifo *f, unsigned long d);

unsigned long fifo_rd(struct fifo *f);

