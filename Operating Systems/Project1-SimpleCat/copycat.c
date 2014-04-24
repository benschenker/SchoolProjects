/*Benjamin Schenker
 * Sept 25, 2013
 * ECE357*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


void readwrite(int fdin, int fdout,int bufsize);
void writeit(int fdout,char* buf, int n);

int main( int argc, char** argv ){
	int oflag = 0;
	char *ovalue = NULL;
	int bflag = 0;
	int bvalue = 128; //default value, changed with -b flag
	int opt;
	int fdi=0;
	int fdo=1;
	char *ifname,*ofname;
	opterr = 0;
	int dashnum=0;

	while ((opt = getopt (argc, argv, "b:o:")) != -1) //parse flags and input/output files
	{
		switch (opt)
		{
			case 'b':
				bflag = 1;
				bvalue=atoi(optarg);
				//fprintf(stdout, "b flag set! value = %d\n",bvalue);
				break;
			case 'o':
				oflag = 1;
				ovalue=optarg;
				//fprintf(stdout, "o flag set! value = %s\n",ovalue);
				if((fdo = open(ovalue, O_RDWR|O_CREAT|O_TRUNC,0644))==-1){//open output file when specified
					int errsv = errno;
					fprintf(stdout, "error opening output file: %s. %s\n",ovalue, strerror(errsv));
					return -1;
				}
				break;
			default: /* '?' */
                   fprintf(stderr, "Usage: %s [-b ###] [-o outfile] [infile1 infile2 ...]\n",argv[0]);
					return -1;
	   }
	}

	int i;
	for(i=optind;i<argc;i++){ //loop through input files
		ifname=argv[i];
		//fprintf(stdout, "index = %d, arg =%s\n",i,argv[i]);
		if(*ifname=='-'){
			readwrite(0,1,bvalue);			
			continue;
		}
		if((fdi = open(ifname, O_RDONLY))!=-1){ //open each input file and perform operations		
			readwrite(fdi,fdo,bvalue);
			int b;
			if(b = close(fdi)){ //close each input file after operations have completed
				int errsv = errno;
				fprintf(stdout, "error closing input file: %s. %s\n",ifname, strerror(errsv));
				return -1;
			}
		}
		else{
			int errsv = errno;
			fprintf(stdout, "error opening input file: %s. %s\n",ifname, strerror(errsv));
			return -1;
		}
	}
	if (fdi==0){ //read from stdin if no input file specified
		readwrite(fdi,fdo,bvalue);
	}
	if(oflag){ //close output file if it was opened
		int b;
		if(b = close(fdo)){
			int errsv = errno;
			fprintf(stdout, "error closing output file: %s. %s\n",ovalue, strerror(errsv));
			return -1;
		}
	}
	return 0;
}

void readwrite(int fdin, int fdout,int bufsize){
	int n;
	char buf[bufsize+1];	
	while((n=read(fdin,buf,bufsize))>0){
		//strcat(buf,"\0");
		writeit(fdout,buf,n);
		
	}
	if(n==-1){
		//error, look at errno
		int errsv = errno;
		fprintf(stdout, "read error. %s\n", strerror(errsv));
	}
};

void writeit(int fdout,char* buf, int n){
	int werr=write(fdout,buf,n);
	if (werr==n){
		//yay
		//fprintf(stdout, "return is correct!\n");
	}
	else if(werr==-1){
			//error, look at errno
			int errsv = errno;
			fprintf(stdout, "write error. %s\n",errsv, strerror(errsv));
	}
	else{
		//didn't write it all...
		fprintf(stdout, "didnt write it all :( trying to finish\n");
		writeit(fdout,buf+werr,n-werr);
	}
}

