/*Benjamin Schenker
 * Nov 6, 2013
 * PS 5
 * ECE357*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

void readwrite(int fdin, int fdout);
void writeit(int fdout,char* buf, int n);

int main( int argc, char** argv ){
	int fd,n;
	char *addr;
	char buf[512], buf2[512];
	struct stat buffer, buffer2;
	
	if (argc<2)
	{
		fprintf(stderr,"error: not enough arguments\n");
		return 1;
	}
	switch(argv[1][0]) //only look at first char of the argument
	{
		case 'A':
			fprintf(stderr,"Problem A:\n");
			if((fd=open("testfile",O_RDONLY))<0)
			{
				perror("open");
				return 1;
			}
			if((addr=mmap(NULL,100,PROT_READ,MAP_PRIVATE,fd,0))==MAP_FAILED)
			{
				perror("map");
				return 1;
			}
			fprintf(stderr,"trying to write 'test' to memory mmapped for read access only\n");
			sprintf(addr,"test\n");
			
			break;
		case 'B':
			fprintf(stderr,"Problem B:\n");
			if((fd=open("testfile",O_RDWR))<0)
			{
				perror("open");
				return 1;
			}
			
			if((addr=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))==MAP_FAILED)
			{
				perror("map");
				return 1;
			}
			fprintf(stderr,"mmap contents before writing to shared mmap: %s",addr);
			sprintf(addr,"testing 12345\n");
			
			if ((n=read(fd,buf,64))<0)
			{
				perror("read error");
			}
			buf[n]='\0';
			fprintf(stderr,"testfile contents after writing to shared mmap: %s\n",buf);
			fprintf(stderr,"YES, update is immediately visible\n");
			break;
		case 'C':
			fprintf(stderr,"Problem C:\n");
			if((fd=open("testfile",O_RDWR))<0)
			{
				perror("open");
				return 1;
			}
			
			if((addr=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0))==MAP_FAILED)
			{
				perror("map");
				return 1;
			}
			fprintf(stderr,"mmap contents before writing to private mmap: %s",addr);
			sprintf(addr,"testing 12345\n");
			
			if ((n=read(fd,buf,64))<0)
			{
				perror("read error");
			}
			buf[n]='\0';
			fprintf(stderr,"testfile contents after writing to private mmap: %s\n",buf);
			fprintf(stderr,"NO, update is not immediately visible\n");
			break;
		case 'D': //and E too
			
			fprintf(stderr,"Problem D:\n");
			if((fd=open("testfile",O_RDWR))<0)
			{
				perror("open");
				return 1;
			}
			fprintf(stderr,"testfile contents are:");
			readwrite(fd,2);
			if (lseek(fd,0,SEEK_SET)<0)//reset file offset to 0
			{
				perror("lseek");
			}
			if((addr=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))==MAP_FAILED)
			{
				perror("map");
				return 1;
			}
			if (fstat(fd,&buffer)<0)
			{
				perror("fstat");
			}
			fprintf(stderr,"filesize before any writing hole is %d\n",buffer.st_size);
			sprintf(addr+buffer.st_size,"hole\n");
			if (fstat(fd,&buffer2)<0)
			{
				perror("fstat");
			}
			fprintf(stderr,"filesize after writing hole is %d\n",buffer2.st_size);
			fprintf(stderr,"So the answer to D is NO, the filesize has not increased\n");
			fprintf(stderr,"Problem E:\nExpanding file past hole\n");
			if (lseek(fd,10,SEEK_END)<0)
			{
				perror("lseek");
			}
			if (write(fd,"new end of file\n",16)<0)
			{
				perror("write");
			}
			if (lseek(fd,0,SEEK_SET)<0)
			{
				perror("lseek");
			}
			fprintf(stderr,"dump is:\n");
			readwrite(fd,2);
			fprintf(stderr,"See, now the hole has appeared!\n");
			break;
		case 'F':
			
			fprintf(stderr,"Problem F:\n");
			if((fd=open("testfile",O_RDWR))<0)
			{
				perror("open");
				return 1;
			}
			if((addr=mmap(NULL,8192,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))==MAP_FAILED)
			{
				perror("map");
				return 1;
			}
			fprintf(stderr,"Accessing first page:\n");
			fprintf(stderr,"%s\n",addr+50);
			fprintf(stderr,"It worked!\nAccessing 2nd page (this will receive SIGBUS\n");
			fprintf(stderr,"%s\n",addr+5000);
			/*Accessing the second page is a valid memory access but the kernel is unable to page in the requested page because the actual file is only 10 bytes and fits on one page.  Since the kernel uses demand paging, it only tries to load the page when we attempt to access it for the first time. So it grabs the first page when we try to access some memory there, but when we try to access memory in the second page, it goes to the file and doesn't see a second page.  Thus it sends a SIGBUS.*/
			break;
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
	while ((0<werr)&&(werr<n))
	{
		
		n-=werr;
		buf+=werr;
		werr=write(fdout,buf,n);
	}
	if (werr<0)
	{
		perror("write error");
	}
}
