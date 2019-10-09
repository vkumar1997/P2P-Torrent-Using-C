#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

#include "mystruct.h"

#define BOOL bool
#define MAX_STRING 200


//check if the argument is a file
bool is_file(char* path) {
	struct stat *buf=malloc(sizeof(stat));
	char temp[200];

	strcpy(temp,path);
	stat(temp, buf);
	bool ans= S_ISREG(buf->st_mode);
	return ans;
}

//check if the argument is a directory
bool is_dir(const char* path) {
	struct stat *buf=malloc(sizeof(stat));
	char temp[200];
	strcpy(temp,path);
	stat(temp,buf);
	return S_ISDIR(buf->st_mode);
}


//return the size of a file
long get_size(const char* path)
{
	struct stat *buf=malloc(sizeof(stat));
	char temp[200];
	strcpy(temp,path);
	stat(temp,buf);
	return buf->st_size;
}



//check if the last character of the fileaddress has a forwar or a backward slash or not
int LastChar(char *str)
{
    return (str && *str && (str[strlen(str) - 1] == '/' || str[strlen(str) - 1] == '\\')) ? 0 : 1;
}





//add a new file to peer
void add_file_to_peer(s_peer *mypeer,char *path,char *filename)
{
	int nchunks,i=0;
	long filesize;

	//file address stored in peer struct
	strcpy(mypeer->all_files[mypeer->no_of_files].fileaddress,path);
	//file name stored in peer struct
	strcpy(mypeer->all_files[mypeer->no_of_files].filename,filename);
	
	filesize = get_size(path);
	
	//if filesize is less than 10 mb, create 10 or 11 chunks
	if(filesize<10*1000*1000)
	{
		nchunks = 10;
		//chunk size stored in peer struct
		mypeer -> all_files[mypeer -> no_of_files].chunk_size = filesize/nchunks;
		//file size stored in peer struct
		mypeer -> all_files[mypeer -> no_of_files].filesize = filesize;
	}

	//filesize greater than 10 mb. create chunks of 1 mb
	else
	{
		//file size stored in peer struct
		mypeer -> all_files[mypeer -> no_of_files].filesize = filesize;
		//chunk size stored in peer struct
		mypeer -> all_files[mypeer -> no_of_files].chunk_size = 1000*1000;
		
	}
	//create number of chunks for each file and store in chunks
	nchunks = (int)(ceil((double)mypeer -> all_files[mypeer -> no_of_files].filesize/ (double)mypeer -> all_files[mypeer -> no_of_files].chunk_size));
	
	//since file present in peer, all chunks will be present
	for(i=0;i<nchunks;i++)
	{
		mypeer -> all_files[mypeer -> no_of_files].chunks_present[i] = true; 	
	}	
	//add file to peer
	mypeer->no_of_files++;
}





//find all files from a directory
void find_files_from_dir(s_peer *mypeer, char* path)
{
	char str[200],temp[200];
	DIR *d;
	int ctr=0,i;
	struct dirent *dir;
	
	
	strcpy(temp,path);
	//open directory
	d = opendir(temp);
	
	//check last character is forward slash, instead add slash
	if(LastChar(temp))
	{
		strcat(temp,"/");
	}

	//check directory
	if (d)
	{
		ctr = 0;
		//check all filename
		while ((dir = readdir(d)) != NULL)
		{
			strcpy(str,temp);
			strcat(str,dir->d_name);
			
			//check if file;
			if(is_file(str))
			{
				//add file to peer
				add_file_to_peer(mypeer, str, dir->d_name);
			}
			
		}
		closedir(d);
	}
}


char* strrev(char* fname)
{
	char temp;
	int i=0;
	int j = strlen(fname)-1;
	while(i<j)
	{
		temp=fname[i];
		fname[i]=fname[j];
		fname[j]=temp;
		i++;
		j--;
	}
	return fname;
}


char* split_file(char* arg,char* fname)
{
	int i=0,ctr=0;
	for(i=strlen(arg)-1;i>=0;i--)
	{
		if(!(arg[i]=='/' || arg[i]=='\\'))
		{
			fname[ctr++]=arg[i];
		}
		else
		{
			break;
		}
	}
	fname[ctr]='\0';
	return fname;
}
