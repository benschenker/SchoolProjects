/*Benjamin Schenker
 * Nov 13, 2013
 * PS 6
 * tcp_send
 * used http://beej.us/guide/bgnet/ as a resource
 * ECE357*/
 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <time.h>
#define MAXBUFLEN 1024

int main( int argc, char** argv ){
	int status, sockfd, sent;
	int mustclose=0;
	int total_trans=0;
	char b[MAXBUFLEN + 1];
	char *buf=b;
	struct addrinfo hints;
	struct addrinfo *res;
	FILE* fp=stdin;
	char *str;
	size_t len;
	struct linger so_linger;
	if (argc<3||argc>4)
	{
		fprintf(stderr,"error: not enough arguments\n");
		return 1;
	}
	
	else if (argc==4)
	{
		char* stok=strtok(argv[3],"<");
		if((fp = fopen(stok, "r"))==NULL)
		{
			perror("infile");
			exit(1);
		}
		mustclose=1;
	}
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
	if ((sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol))<0)
	{
		perror("socket");
		exit(1);
	}
	so_linger.l_onoff = 1;
	so_linger.l_linger=5;
	if (setsockopt(sockfd,SOL_SOCKET, SO_LINGER,&so_linger,sizeof so_linger)!=0)
	{
		perror("SO_LINGER");
		exit(1);
	}
	if (connect(sockfd, res->ai_addr, res->ai_addrlen)==-1)
	{
		perror("connect");
		exit(1);
	}
	struct timespec *tp1=(struct timespec*)malloc(sizeof(struct timespec*));
	struct timespec *tp2=(struct timespec*)malloc(sizeof(struct timespec*));
	clock_gettime(CLOCK_REALTIME,tp1);
	while ((str = fgets(buf, MAXBUFLEN, fp))!=NULL)
	{
		len=strlen(str);
		sent=send(sockfd,buf,len,0);
		total_trans+=sent;
		while((0<sent)&&(sent<len))
		{
			len-=sent;
			buf+=sent;
			sent=send(sockfd,buf,len,0);
			total_trans+=sent;
		}
		if(sent<0)
		{
			perror("send");
			exit(1);
		}
	}
	clock_gettime(CLOCK_REALTIME,tp2);
	if (len==-1)
	{
		perror("read");
	}
	
	double timediff = (double)((tp2->tv_sec*1000000000+tp2->tv_nsec)-((tp1->tv_sec*1000000000+tp1->tv_nsec)))/1000000000;
	double datarate = (double)total_trans/timediff ;
	fprintf(stderr, "rate = %fMB/s\n",datarate/1000000);
	if(mustclose)
	{
		fclose(fp);
	}
	return 0;
}
