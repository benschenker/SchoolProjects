/*Benjamin Schenker
 * October 16, 2013
 * ECE357
 * Problem Set 3*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>

int makeredirs(char* redir);

int main( int argc, char** argv )
{
	char* stok;
	int script=0;
	FILE* istream=stdin; 
	if (argc>1)
	{
		if ((istream = fopen(argv[1], "r")) == 0)
		{
			fprintf(stderr, "%s: failed to open file %s\n", argv[0], argv[1]);
			return 1;
		}
		else
		{
			script=1;
		}
	}
	char buf[4096];
	while (fgets(buf,sizeof(buf),istream))
	{
		if(buf[0]=='#')
		{
			continue;
		}
		char* stok=strtok(buf," \t\n");
		char* progargv[10];
		char* redirs[2];
		int redirnum=0;
		int progargc=0;
		while (stok!=NULL)
		{
			if ((stok[0]=='<')||(stok[0]=='>')||(stok[0]=='2'))
			{
				redirs[redirnum]=(char*)malloc(sizeof(char*));
				redirs[redirnum]=stok;
				redirnum++;
				stok=strtok(NULL, " \t\n");
				continue;
			}
			progargv[progargc]=(char*)malloc(sizeof(char*));
			progargv[progargc]=stok;
			stok=strtok(NULL, " \t\n");
			progargc++;
		}
		if (progargc>1)//with arguments
		{
			printf("Executing command %s with arguments '",progargv[0]);
			int i;
			for(i=1;i<progargc;i++)
			{
				if (i+1==progargc)
				{
					printf("%s",progargv[i]);
				}
				else
				{
					printf("%s ",progargv[i]);
				}
			}
			printf("'\n");
		}
		else //no arguments
		{
			printf("Executing command %s\n",progargv[0]);
		}
		int pid;
		struct timespec *tp1=(struct timespec*)malloc(sizeof(struct timespec*));
		struct timespec *tp2=(struct timespec*)malloc(sizeof(struct timespec*));
		if (clock_gettime(CLOCK_REALTIME,tp1)<0)
		{
			perror("can't get real time");
		}
		switch(pid=fork())
		{
			case -1:
				perror("fork failed");exit(1);
				break;
			case 0:
				switch(redirnum)
				{
					case 0: //no redirection
						break;
					case 1:
						if(makeredirs(redirs[0]))
						{
							printf("redirection on %s failed\n",redirs[0]);
						}
						break;
					case 2:
						if(makeredirs(redirs[0])||makeredirs(redirs[1]))
						{
							printf("redirection on %s and %s failed\n",redirs[0],redirs[1]);
						}
						break;
					default :
						printf("too many redirections!\n");
						return 1;

				}
				if (script)
				{
					fclose(istream);
				}
				if(execvp(progargv[0],progargv))
				{
					perror("exec failed");exit(1);
				}
				break;
			default :
			{
				struct rusage ru;
				int status;
				if (wait3(&status,0,&ru)==-1)
				{
					perror("wait3");
				}
				else
				{
					if (clock_gettime(CLOCK_REALTIME,tp2)<0)
					{
						perror("can't get real time");
					}
					int seconds=tp2->tv_sec - tp1->tv_sec;
					int nanoseconds=tp2->tv_nsec - tp1->tv_nsec;
					if (nanoseconds<0) // to carry
					{
						seconds--;
						nanoseconds= 1000000000+nanoseconds;
					}
					fprintf(stderr, "Consuming %ld.%.9d real seconds, %1d.%.6d user,%1d.%.6d system time\n",seconds,nanoseconds,ru.ru_utime.tv_sec,ru.ru_utime.tv_usec,ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
				}
				break;
			}
		}
	}
	if (script)
	{
		fclose(istream);
	}
	return 0;
}

int makeredirs(char* redir)
{
	//redir starts with '<' '>' or '2'
	int len =strlen(redir);
	int fd=0;	
	int flags;
	char* stok=strtok(redir,"<>2");
	switch(redir[0])
	{
		case '<':
		{
			flags=O_RDONLY;
			if((fd=open(stok,flags))<0)
			{
				perror("error opening input redir file");
				return 1;
			}
			else
			{
				if (dup2(fd,0)<0)
				{
					perror("error duping");
					return 1;
				}
			}
			break;
		}
		case '>':
			if (redir[1]=='>')
			{
				flags=O_WRONLY|O_CREAT|O_APPEND;
			}
			else
			{
				flags=O_WRONLY|O_CREAT|O_TRUNC;
			}
			if((fd=open(stok,flags,0644))<0)
			{
				perror("error opening output redir file");
				return 1;
			}
			else
			{
				if (dup2(fd,1)<0)
				{
					perror("error duping");
					return 1;
				}
			}
			break;
		case '2':
			if (redir[1]=='>')
			{
				flags=O_WRONLY|O_CREAT|O_APPEND;
			}
			else
			{
				flags=O_WRONLY|O_CREAT|O_TRUNC;
			}
			if((fd=open(stok,flags,0644))<0)
			{
				perror("error opening err redir file");
				return 1;
			}
			else
			{
				if (dup2(fd,2)<0)
				{
					perror("error duping");
					return 1;
				}
			}
			break;
	}
	if(fd>0)
	{
		if (close(fd))
			{
				perror("couldn't close redir file");
				return 1;
			}
	}
	return 0;
}
