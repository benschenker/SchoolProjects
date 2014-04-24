/*Benjamin Schenker
 * October 9, 2013
 * ECE357
 * Problem Set 2*/
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

int find(char* name,char* rootname, int uvalue,int mvalue);

int main( int argc, char** argv )
{
	//char* name = ".";
	char *ustring = NULL;
	int uvalue=0;
	char *mstring = NULL;
	int mvalue=0;
	int opt;

	while ((opt = getopt (argc, argv, "u:m:")) != -1) //parse flags and input/output files
	{
		switch (opt)
		{
			case 'u':
				ustring=optarg;
				//if it is an int convert, else find the uid corresponding to username
				if (sscanf(ustring, "%d",&uvalue)==1)
				{
					uvalue=atoi(ustring);
				}
				else
				{
					errno=0;
					struct passwd *pw = getpwnam(ustring);
					if(pw)
					{
						uvalue=pw->pw_uid;
					}
					else if (errno)
					{
						printf("Error, invalid username %s: %s\n",ustring,strerror(errno));
						return 0;
					}
					else //no error but username doesn't exist on system
					{
						printf("Error, invalid username %s\n",ustring);
						return 0;
					}
				}
				break;
			case 'm':
				mstring=optarg;

				if (sscanf(mstring, "%d", &mvalue)==1)
				{
					mvalue=atoi(mstring);
				}
				else
				{
					printf("Error, invalid time format %s\n",mstring);
					return 0;
				}
				break;
			default: /* '?' */
                   fprintf(stderr, "Usage: %s [-u user] [-m mtime] starting_path\n",argv[0]);
					return -1;
	   }
	}
	if (optind<argc) // if directory specified, use it, else default to cwd
	{
		if(!find(argv[optind],argv[optind],uvalue,mvalue))
		{
			return 0;
		}
	}
	else
	{
		if (!(find(".",".",uvalue,mvalue)))
		{
			return 0;
		}
	}
	return 0;
}
int find(char *filename,char* rootname,int uvalue,int mvalue)
{
	struct stat buffer;
	DIR *dirp;
	struct dirent *de;
		if (!(dirp=opendir(filename)))
		{
			printf("Can not open directory %s:%s\n",filename,strerror(errno));
			return 0;
		}
		while (de=readdir(dirp))
		{
			char* pathname = (char*)malloc(512);
			if (!strcmp(de->d_name,"..")) //avoiding parent directory
			{
				continue;
			}
			else if (!strcmp(de->d_name,"."))//renaming "." the first time it is encountered then ignoring it
			{
				if (!strcmp(filename,rootname))
				{
					strcpy(pathname,filename);
				}
				else	{continue;}
			}
			else
			{
			strcpy(pathname,filename);
			strcat(pathname,"/");
			strcat(pathname,de->d_name);
			}
//initializing variables that detemine whether certain aspects get displayed
			int isdir=0;
			int ischarblock=0;
			int islink=0;
			
			if (lstat(pathname,&buffer))
			{
				printf("Can not lstat item %s:%s\n",de->d_name,strerror(errno));
				return 0;
			}
			if ((uvalue)&&(buffer.st_uid!=uvalue)) {continue;}
			if (((mvalue>0)&&(buffer.st_mtime+mvalue>time(0)))||(mvalue<0)&&(buffer.st_mtime-mvalue<time(0)))
			{continue;}
//printing dev and inode number
			printf("%.4x/%d ",buffer.st_dev,buffer.st_ino);
//printing permissions mode, read more on http://www.grymoire.com/Unix/Permissions.html#toc-uh-9
			if (S_ISREG(buffer.st_mode)) printf("-");
			else if (S_ISDIR(buffer.st_mode)) 
			{
				printf("d");
				if (strcmp(de->d_name,".")&&strcmp(de->d_name,".."))
				{
					isdir=1;
				}
			}
			else if (S_ISCHR(buffer.st_mode)) {printf("c"); ischarblock=1;}
			else if (S_ISBLK(buffer.st_mode)) {printf("b"); ischarblock=1;}
			else if (S_ISFIFO(buffer.st_mode)) printf("f");
			else if (S_ISLNK(buffer.st_mode)) {printf("l"); islink=1;}
			else if (S_ISSOCK(buffer.st_mode)) printf("s");
			printf( (buffer.st_mode & S_IRUSR) ? "r" : "-");
			printf( (buffer.st_mode & S_IWUSR) ? "w" : "-");
			if (buffer.st_mode & S_ISUID) //check for user setuid bit, else check for ux permissions
			{
				printf( (buffer.st_mode & S_IXUSR) ? "s" : "S");
			}
			else
			{
				printf( (buffer.st_mode & S_IXUSR) ? "x" : "-");
			}
			
			printf( (buffer.st_mode & S_IRGRP) ? "r" : "-");
			printf( (buffer.st_mode & S_IWGRP) ? "w" : "-");
			if (buffer.st_mode & S_ISGID)//check for group setuid bit, else check for gx permissions
			{
				printf((buffer.st_mode & S_IXGRP) ? "s" : "S");
			}
			else
			{
				printf( (buffer.st_mode & S_IXGRP) ? "x" : "-");
			}
			printf( (buffer.st_mode & S_IROTH) ? "r" : "-");
			printf( (buffer.st_mode & S_IWOTH) ? "w" : "-");
			if (buffer.st_mode & S_ISVTX)//check for sticky bit, else check for ox permissions
			{
				printf((buffer.st_mode & S_IXOTH) ? "t" : "T");
			}
			else
			{
				printf( (buffer.st_mode & S_IXOTH) ? "x" : "-");
			}
//printing number of hard links
			printf(" %d ",buffer.st_nlink);
//printing user owner
			struct passwd *pw=getpwuid(buffer.st_uid);
			if(pw)
			{
				printf("%s    ",pw->pw_name);
			}
			else if (errno==0)
			{
				printf("%d    ",buffer.st_uid);
			}
			else
			{
				printf("Error in file %s, %s\n",de->d_name,strerror(errno));
				return 0;
			}
//printing group owner
			struct group *gr=getgrgid(buffer.st_gid);
			if(gr)
			{
				printf("%s    ",gr->gr_name);
			}
			else if (errno==0)
			{
				printf("%d    ",buffer.st_gid);
			}
			else
			{
				printf("Error in file %s, %s\n",de->d_name,strerror(errno));
				return 0;
			}
//printing size of node
			if(ischarblock)
			{
				printf("%x ",buffer.st_rdev);
			}
			else
			{
				printf("%d ",buffer.st_size);
			}
//printing time
			char* time= ctime(&(buffer.st_mtime));
			time[24]=' '; //removing newline
			printf("%s",time);
//printing directory path
			printf("%s\n",pathname);
//printing link target
			if (islink)
			{
				char* linkpath = (char*)malloc(512);
				int numbyte=readlink(pathname,linkpath,512);
				if(numbyte==-1)
				{
					printf("Error following link %s:%s\n",pathname,strerror(errno));
					return 0;
				}
				else
				{
					linkpath[numbyte]='\0';
					printf("  ->%s\n",linkpath);
				}
				free(linkpath);
			}
//recursively call itself on directory after encountering directory
			if (isdir)
			{
				if (!find(pathname,filename,uvalue,mvalue))
				{
					return 0;
				}
			}
			free(pathname);
		}
		closedir(dirp);
}
