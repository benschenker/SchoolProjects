/*Benjamin Schenker
 * Nov 26, 2013
 * PS 8
 * p5.c
 * ECE357*/
 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#define BIGNUM 10000000L

void empty(){}
void sys()
{
	getuid();
}
int main( int argc, char** argv ){

	struct timespec *tp1=(struct timespec*)malloc(sizeof(struct timespec*));
	struct timespec *tp2=(struct timespec*)malloc(sizeof(struct timespec*));
	long i;
	
	switch(*argv[1])
	{
		case 'A':
			clock_gettime(CLOCK_REALTIME,tp1);
			for (i = 0; i < BIGNUM; i++)
			{}
			clock_gettime(CLOCK_REALTIME,tp2);
			break;
		case 'B':
			clock_gettime(CLOCK_REALTIME,tp1);
			for (i = 0; i < BIGNUM; i++)
			{
				empty();
			}
			clock_gettime(CLOCK_REALTIME,tp2);
			break;
		case 'C':
			clock_gettime(CLOCK_REALTIME,tp1);
			for (i = 0; i < BIGNUM; i++)
			{
				sys();
			}
			clock_gettime(CLOCK_REALTIME,tp2);
			break;
		default:
			fprintf(stderr,"2nd arg must be 'A' 'B' or 'C'\n");
			exit(1);
	}



	double timediff = (double)((tp2->tv_sec*1000000000+tp2->tv_nsec)-((tp1->tv_sec*1000000000+tp1->tv_nsec)))/1000000000;
	fprintf(stderr, "time = %fs\n",timediff);
	
	return 0;
}
