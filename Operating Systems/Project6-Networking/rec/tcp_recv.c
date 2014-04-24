/*Benjamin Schenker
 * Nov 13, 2013
 * PS 6
 * tcp_recv
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
	int new_fd, sockfd, rec, status;
	int mustclose=0;
	int total_trans=0;
	char b[MAXBUFLEN + 1];
	char *buf=b;
	struct addrinfo hints, *res;
	FILE* fp=stdout;
	size_t len;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	if (argc<2||argc>3)
	{
		fprintf(stderr,"error: not enough arguments\n");
		return 1;
	}
	
	else if (argc==3)
	{
		char* stok=strtok(argv[2],">");
		if((fp = fopen(stok, "w"))==NULL)
		{
			perror("outfile");
			exit(1);
		}
		mustclose=1;
	}
	memset(&b, 0, sizeof b);
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
	if ((sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol))<0)
	{
		perror("socket");
		exit(1);
	}
	if (bind(sockfd,res->ai_addr,res->ai_addrlen)==-1)
	{
		perror("bind");
	}
	if (listen(sockfd, 10)==-1)
	{
		perror("listen");
		exit(1);
	}
	addr_size=sizeof their_addr;
	if ((new_fd=accept(sockfd,(struct sockaddr *)&their_addr,&addr_size))==-1)
	{
		perror("accept");
	}
	struct timespec tp1;
	struct timespec tp2;
	clock_gettime(CLOCK_REALTIME,&tp1);

	while ((rec = recv(new_fd, buf, MAXBUFLEN, 0))>0)
	{		
		if (fputs(buf,fp)!=EOF)
		{
			total_trans+=rec;
			memset(&b, 0, sizeof b);
		}
	}
	if (rec==-1)
	{
		perror("recv");
	}
	if(close(new_fd)||close(sockfd))
	{
		perror("close");
	}
	if(mustclose)
	{
		fclose(fp);
	}
	if (clock_gettime(CLOCK_REALTIME,&tp2)<0)
	{
		perror("can't get real time");
	}
	double timediff = (double)((tp2.tv_sec*1000000000+tp2.tv_nsec)-
		((tp1.tv_sec*1000000000+tp1.tv_nsec)))/1000000000;
	double datarate = (double)total_trans/timediff ;
	fprintf(stderr, "bytes received = %d, rate = %fMB/s\n",total_trans, datarate/1000000);
	char s[INET6_ADDRSTRLEN], host[1024], service[20];
    getnameinfo(((struct sockaddr*)&their_addr), sizeof their_addr, 
    	host, sizeof host, service,sizeof service,0);
    fprintf(stderr,"IP:%s, hostname = %s\n",
        inet_ntop(their_addr.ss_family,&(((struct sockaddr_in*)&their_addr)->sin_addr),
        s, sizeof s),host);
	return 0;
}
