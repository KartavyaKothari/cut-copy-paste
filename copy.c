#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<time.h>

float size=0;

//prototyping functions to be used
void copyfile(char start[],char end[]);//copies a single file from source path to destination 
void traverse(char source[],char destination[]);//traverses directories
int isdir(char path[]);//checks a string is a pathname or a directory

int main(int argc,char *argv[])//The program will accept command line arguments to simulate a terminal command
{
	time_t start=time(NULL);

	if(argc<3)//check for proper number of aruguments 
	{
		printf("ERROR: Improper syntax\n");
		printf("\t./copy [source path] [destination path]\n");//expected syntax
		exit(0);
	}

	char *source=(char *)malloc(strlen(argv[1])+1);//creating variable for source destination for manupulation
	char *destination=(char *)malloc(strlen(argv[2])+1);//and destination for manipulation

	strcpy(source,argv[1]);//assigning 
	strcpy(destination,argv[2]);//strings to variables

	/*There are 4 simple cases to be handled
		0 - not a directory
		1 - directory

	{case}    {SOURCE} {DESTINATION} {CAN BE COPIED}

	1] 	  0           0             1		
	2] 	  0           1	       	    1
	3] 	  1           0             0 	- a directory cannot be copied on a file
        4] 	  1           1             1

	*/

	if(isdir(source)&&!isdir(destination))//To eliminate case 3]
	{
		printf("Destination cannot be a file\n");
		exit(0);
	}
	
	if(isdir(source)&&isdir(destination))//Case 4] requires traversing
		traverse(source,destination);
	else copyfile(source,destination);//Case 1] and 2] no traversing required, direct copyfile

	time_t end=time(NULL);
	int size_o=size;
	char type[10];
	if(size<1024)strcpy(type,"Bytes");
		else if((size=size/1024)<1024)strcpy(type,"Kilobytes");
			else if((size=size/1024)<1024)strcpy(type,"Megabytes");
				else {size=size/1024;strcpy(type,"Gigabytes");}

	printf("Total size of files copied = %.2f %s\n",size,type);
	printf("Average speed = %.2f mb/secs\n",(float)size_o/difftime(end,start)/1024/1024);

	return 0;
}

int isdir(char path[])
{
	struct stat dir;
	stat(path,&dir);
	if(S_ISREG(dir.st_mode))//checks if string is file
		return 0;
	if(S_ISDIR(dir.st_mode))//checks if string is directory
		return 1;
	printf("The source is not a file, neither a dir\n");
	exit(0);
}

void copyfile(char start[],char end[])
{
	int c;	

	if(!isdir(start)&&isdir(end))//Converts Case 2] to Case 1]
	{//Before conversion destination == /xxxx/xxxx/xxxx
		char *fname=strrchr(start,'/');
		end=realloc(end,strlen(end)+strlen(start)+2);
		if(end==NULL)
		{
			printf("Memory reallocation error\n");
			exit(0);
		}
		strcat(end,fname);	
	//After conversion destination == /xxxx/xxxx/xxxx/[filename.extension]
	}

	//Opening source and destination files
	FILE *f1=fopen(start,"r");
	if(f1==NULL){printf("File does not exists");exit(0);}
	FILE *f2=fopen(end,"w");
	if(f2==NULL){printf("File copying error: Directory may or may not exist\n");exit(0);}

	while((c=fgetc(f1))!=EOF)fputc(c,f2);//Core copy instruction

	fclose(f1);
	fclose(f2);
	return;
}

void traverse(char source[],char destination[])
{	
	char *fname=strrchr(source,'/');//Get the name of target folder
	
	char *sys=(char *)malloc(strlen(destination)+strlen(fname)+1+6+2);//String buffer to pass to system()

	strcpy(sys,"mkdir \"");
	strcat(sys,destination);
	strcat(sys,fname);
	strcat(sys,"\"");
	system(sys);

	DIR *d1;
	if((d1=opendir(source))==NULL)
	{
		printf("Source access inturrupted\n");
		exit(0);
	}

	struct dirent *cwd;
	struct stat dir;
	
	while((cwd=readdir(d1))!=NULL)
	{	
		char *tsource=(char *)malloc(strlen(source)+strlen(cwd->d_name)+2);
		char *tdestination=(char *)malloc(strlen(destination)+strlen(fname)+1);
		strcpy(tdestination,destination);
		strcat(tdestination,fname);
		
		strcpy(tsource,source);
		strcat(tsource,"/");
		strcat(tsource,cwd->d_name);

		if(isdir(tsource))
		{
			if(strcmp(cwd->d_name,".")==0||strcmp(cwd->d_name,"..")==0)continue;		
			traverse(tsource,tdestination);continue;
		}
		stat(tsource,&dir);
		size=size+dir.st_size;
		copyfile(tsource,tdestination);
	}
		
	closedir(d1);	

	return;
}
