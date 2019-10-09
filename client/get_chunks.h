#include "download_thread.h"
#include "add_new_file.h"
#include "chunk_support.h"
#include "rarest_chunk.h"


//get file chunks in a loop
void * get_file_chunks(void* args/*int sockfd, char* filename, s_location_reply *reply,s_peer* retpeer,long filesize*/)
{
	//socket for tracker.. for updating chunk status
	int sockfd;
	char filename[200];
	s_peer * retpeer = malloc(sizeof(s_peer));
	long filesize;
	s_message_type *file = malloc(sizeof(s_message_type));
	file->type = 0;

	//download thread
	s_download_thread *dthread = (s_download_thread*)args;
	sockfd = dthread->sockfd;
	strcpy(filename, dthread->filename);
	retpeer = dthread->peer;
	filesize = dthread->filesize;
	int index = -1;
	
	//if file not partially downloaded
	if(dthread->partial_download<0)
	{
		index = add_filename(retpeer,filename,filesize); //add file to peer and find index
		file->download_file.partial_download = false;

	}
	else
	{
		index = dthread->partial_download; //index of partial downloaded file
		file->download_file.partial_download=true;
	}
	

	printf("Filename: %s\n",filename);
	strcpy(file->download_file.filename,filename);
	file->download_file.filesize=filesize;
		
	
	//struct for finding locations of chunk
	s_location_reply *reply;	
	reply = malloc(sizeof(s_location_reply));
	reply->no_of_ipaddress=0;
	//counter for iterations
	int check = 0;

	//location update after every th iterations
	int th = 20;
	while(1)
	{
		//find locations of chunks of a file
		//printf("Check: %d\n",check);
		if(check%th==0)
		{
			s_location_reply *rep;	
			rep = malloc(sizeof(s_location_reply));	
			rep->no_of_ipaddress=0;
			//request for a file from the tracker
			write(sockfd,file,sizeof(s_message_type));
			
			//read location of chunks of files from tracker
			int size=read(sockfd,rep,sizeof(s_location_reply));
			
			//if location struct not empty
			//printf("nop %d\n", rep->no_of_ipaddress);
			if(rep->no_of_ipaddress>0 && size==sizeof(s_location_reply))
			{
				reply=rep;
			}
		}

		//update counter
		check++;
		
		//find rarest chunk and download
		//printf("Check: %d\n",check);
		printf("\nNumber of peers: %d\n", reply->no_of_ipaddress);
		if(reply->no_of_ipaddress>0)
		{
			if(rarest_chunk(reply,retpeer,index,filename,sockfd))
				break;
			th=20;
		}

		//if location reply not found, update location struct sooner
		else
		{
			th=3;
		}	
		sleep(1);
	}

}