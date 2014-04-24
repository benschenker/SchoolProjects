/*Benjamin Schenker
 * Oct 30, 2013
 * ECE357*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
void readwrite(int fdin, int fdout);
void writeit(int fdout,char* buf, int n);

int fproc=0;
int bproc=0;

void sighand(int sig)
{
	fprintf(stderr,"files processed: %d, bytes processed: %d.\n",fproc,bproc);
	exit(0);
}

int main( int argc, char** argv ){
	int pipe1[2], pipe2[2], fdi;
	char *ifname;
	if (argc<2)
	{
		fprintf(stderr,"error: not enough arguments\n");
	}
	signal(SIGINT,sighand);
	signal(SIGPIPE,sighand);
	int i;
	for(i=2;i<argc;i++)
	{ //loop through infiles
		ifname=argv[i];
		if((fdi = open(ifname, O_RDONLY))!=-1)
		{
			fproc++;
			if (pipe(pipe1)||pipe(pipe2))
			{
				perror("can't create pipe");
				return 1;
			}
			switch(fork())
			{
				case -1:
					perror("fork failed");exit(1);
					break;
				case 0:
					if ((dup2(pipe1[0],0))||(dup2(pipe2[1],1)<0))
					{
						perror("error duping");
					}
		
					if(close(fdi)||close(pipe1[0])||close(pipe1[1])||close(pipe2[0])||close(pipe2[1]))
					{
						perror("error closing files");
						return 1;
					}
		
					if (execlp("grep","grep",argv[1],NULL))
					{
						perror("exec failed");
						return 1;
					}
					break;
				default :
					switch(fork())
					{
						case -1:
							perror("fork failed");exit(1);
							break;
						case 0:
							if (dup2(pipe2[0],0)<0)
							{
								perror("error duping");
							}
				
							if(close(fdi)||close(pipe1[0])||close(pipe1[1])||close(pipe2[0])||close(pipe2[1]))
							{
								perror("error closing files");
								return 1;
							}
				
							if (execlp("more","more",NULL))
							{
								perror("exec failed");
								return 1;
							}
							break;
						default :
							if(close(pipe1[0])||close(pipe2[0])||close(pipe2[1]))
							{
								perror("error closing input file");
								return 1;
							}
							readwrite(fdi,pipe1[1]);
							if(close(fdi)||close(pipe1[1]))
							{
								perror("error closing input file");
								return 1;
							}
							if (wait(NULL)<0)
							{
								perror("wait had an error");
							}
					
							break;
					}
					if (wait(NULL)<0)
					{
						perror("wait had an error");
					}
						
					break;
			}
		}
	}
	return 0;
}

void readwrite(int fdin, int fdout)
{
	int n;
	int bufsize=1024;
	char buf[bufsize+1];	
	while((n=read(fdin,buf,bufsize))>0)
	{
		writeit(fdout,buf,n);
	}
	if(n==-1)
	{
		perror("read error");
	}
};
void writeit(int fdout,char* buf, int n)
{
	int werr=write(fdout,buf,n);
	bproc+=werr;
	while ((0<werr)&&(werr<n))
	{
		
		n-=werr;
		buf+=werr;
		werr=write(fdout,buf,n);
		bproc+=werr;
	}
	if (werr<0)
	{
		perror("write error");
	}
}
