/*Benjamin Schenker
 * Nov 13, 2013
 * PS 6
 * udp_server
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
#include <sys/sysinfo.h>
#define MAXBUFLEN 1024

int main( int argc, char** argv ){
	int new_fd, sockfd, rec, status;
	char b[MAXBUFLEN + 1];
	
	char *buf=b;
	struct addrinfo hints, *res;
	FILE* fp=stdout;
	size_t len;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct sysinfo info;
	time_t curtime;
	if (argc<2||argc>2)
	{
		fprintf(stderr,"error: not enough arguments\n");
		return 1;
	}
	memset(&b, 0, sizeof b);
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
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
	addr_size=sizeof their_addr;
	while ((rec = recvfrom(sockfd, buf, MAXBUFLEN, 0, (struct sockaddr *)&their_addr,&addr_size))>0)
	{		
		char *msg = (char*)malloc(sizeof(char*));
		if (!strcmp(buf,"UPTIME"))
		{	
			if(sysinfo(&info)<0)
			{
				perror("sysinfo");
				exit(1);
			}
			sprintf(msg,"%lu",info.uptime);
			
		}
		else if (!strcmp(buf,"DATE"))
		{
			time(&curtime);
			msg=ctime(&curtime);
		}
		if(sendto(sockfd,msg,strlen(msg),0, (struct sockaddr *)&their_addr,addr_size)<0)
		{
			perror("sendto");
			exit(1);
		}
		memset(&b, 0, sizeof b);
	}
	if (rec==-1)
	{
		perror("recv");
	}
	if(close(sockfd))
	{
		perror("close");
	}
	
	return 0;
}
