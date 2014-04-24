/*Benjamin Schenker
 * Nov 13, 2013
 * PS 6
 * udp_client
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
	int status, sockfd, sent,rec;
	char b[MAXBUFLEN + 1];
	char *buf=b;
	struct addrinfo hints;
	struct addrinfo *res;
	FILE* fp=stdin;
	char *str;
	size_t len;
	if (argc<4||argc>4)
	{
		fprintf(stderr,"error: not enough arguments\n");
		return 1;
	}
	str=argv[3];
	if(strcmp(str,"DATE")&&strcmp(str,"UPTIME"))
	{
		fprintf(stderr,"only supports 'UPTIME' and 'DATE'\n");
		exit(1);
	}
	memset(&b, 0, sizeof b);
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
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
		len=strlen(str);
		if(sendto(sockfd,str,len,0, res->ai_addr,res->ai_addrlen)<0)
		{
			perror("sendto");
			exit(1);
		}
		if(recvfrom(sockfd, buf, MAXBUFLEN, 0, res->ai_addr,&(res->ai_addrlen))<0)
		{
			perror("recvfrom");
			exit(1);
		}
		fprintf(stderr,"response = %s\n",buf);
	return 0;
}
