#include <unistd.h> //for NULL pointer
#include <stdio.h> //standard input and output oprerations
#include <stdlib.h> //standard library for C
#include <netinet/in.h> //for socket address
#include <arpa/inet.h> //for ip addres
#include <sys/time.h> 
#include <sys/socket.h>//for reading writing opening closing binding listening and connecting sockets 
#include <pthread.h> //for multiple threads
#include <sys/stat.h> //
#include <fcntl.h> //file operations
#include <pthread.h> //for math operations
#include <math.h> //for directories and files

pthread_rwlock_t rwlock; //for locking file while writing operation
#include "myfunc.h" //for checking files and folders using the agrument provided by the user
#include "socket.h" //socket operations
#include "get_chunks.h" //downloading chunks of a file
#include "peer_serve.h" //uploading chunks of a file
#include "register_files.h" //registering files 

//number of string characters allowed
#define MAX 128 //max number of peers allowed to connect at a time	
#define PORT 8040 //port to connect to at the tracker
#define IPADDRESS "127.0.0.1" //ip adress of the tracker


s_peer *mypeer,*retpeer; //peer structure contatining ip address, port, files and other things to be sent to the tracker
s_download_thread *dthread; //download thread structure to download files from other peers

int main(int argc, char *argv[])
{
	mypeer = malloc(sizeof(s_peer)); //create a peer struct. see mystruct.h
	retpeer = malloc(sizeof(s_peer)); //create a peer struct. see mystruct.h
	mypeer->no_of_files=0; //assign 0 for number of files initially
	retpeer->no_of_files=0; //assign 0 for number of files initially
	
	dthread = malloc(sizeof(s_download_thread)); //download thread

	int sockfd,connfd,len,ctr,i;
	struct sockaddr_in servaddr,cli; //for storing socket addresses
	pthread_t d_th; //download thread
	strcpy(mypeer->ipaddress,IPADDRESS); //assign IP Address
	//srand(time(0));
	
	mypeer->no_of_files=0; //assign number of files
	


	//READING FILE OR FOLDER FROM ARGUMENT

	if(argc>2)
	{	
		if(is_file(argv[2])) //if is file
		{
			char fname[200];
			split_file(argv[2],fname); //get the file name from the whole address
			strcpy(fname,strrev(fname)); 
			add_file_to_peer(mypeer,argv[2],fname); //add file
		}
		
		else if(is_dir(argv[2])) //if is directory
		{
			find_files_from_dir(mypeer,argv[2]); //add all files
	 	}

	 	//show information of all files recorded
	    if(mypeer->no_of_files!=0)
	    {
		    printf("List of files to be sent:\n");
		    for(i=0; i < mypeer->no_of_files; i++ )
		    {
		    	printf("Filename: %s || ", mypeer->all_files[i].filename);
		    	printf("Chunk size: %d || ", mypeer->all_files[i].chunk_size);
		    	printf("File size: %ld || \n", mypeer->all_files[i].filesize);	
		    }	
		}

	}

	if(argc>1)//check if port to connect with other peers was provided otherwise exit program
	{
		if(atoi(argv[1])==0) //convert argument (port) to integet
			return 1;

		mypeer->port = atoi(argv[1]);
		printf("port: %d\n", mypeer->port);
		
		sockfd = createsocket();
		if (sockfd<0) //create socket for connection
			return 1;

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //ipaddress to connect to tracker
		servaddr.sin_port = htons(PORT); //port to connect to tracker
		if(connectsocket(sockfd,servaddr)<0)  //connect to tracker
			return 1;


		int if_partial_download = register_files_with_server(sockfd,mypeer,retpeer,dthread); //if file partially downloaded. Register files with the tracker
		write(sockfd,retpeer,sizeof(s_peer)); //ask tracker for file list
		s_file_list *fileenv; //see mystruct.h for details. structure for file list reply
		fileenv = malloc(sizeof(s_file_list));	
		fileenv->no_of_files=0;
		read(sockfd,fileenv,sizeof(s_file_list)); //read file list reply
		dthread->partial_download = if_partial_download;	//if file partially downloaded, send for complete download
		
		if(if_partial_download<0) //if file not partially downloaded
		{
			printf("\nDo you want to download any files (y/n):\n");  //if want to download file.
			char c; 
			while(true) //loop for accepting character
			{
				c=getchar();//accepting charater
				if(c=='y') //if yes
				{
					if(fileenv->no_of_files>0) //see if  files available for download
					{
						printf("\n\nFiles you can download:\n");
						for(i=0;i<fileenv->no_of_files;i++) //print files that can be downloaded
						{
							printf("[%d]: %s 	%ldB\n", i+1,fileenv->filelist[i].filename,fileenv->filelist[i].filesize);
						}

						int filenumber=0;
						printf("Enter the file number to download: ");
						scanf("%d",&filenumber); //accept the filenumber that is to be downloaded

						dthread->sockfd = sockfd; //prepare download thread for download
						strcpy(dthread->filename,fileenv->filelist[filenumber-1].filename); //copy filename to download thread
						dthread->peer = retpeer; //copy peer information to download thread
						dthread->filesize = fileenv->filelist[filenumber-1].filesize; //copy filesize to download thread
						if(pthread_create(&d_th,NULL,get_file_chunks,dthread)) // create download thread
							printf("failed to create thread\n");
					}
					else
					{
						printf("You dont have any files to download\n"); //else dont have any files for download if file list reply is returned empty
						s_message_type *message = malloc(sizeof(s_message_type));	//return empty message to let the tracker know that the peer will not be downloading any files
						message->type = -1;
						write(sockfd,message,sizeof(s_message_type)); 
						
					}
					break;
				}

				else if(c=='n') //if answer is no
				{
					s_message_type *message = malloc(sizeof(s_message_type)); //return empty message to let the tracker know that the peer will not be downloading any files
					message->type = -1;
					write(sockfd,message,sizeof(s_message_type));
					break;
				}

				else if(c=='\n') //bug tolerance
					continue;
				else //avoiding garbage user input
				{
					printf("Enter a valid value\n");
					continue;
				}
			}
		}
		
		else //create thread to download partially downloaded files
		{
			printf("%s\n", dthread->filename);
			if(pthread_create(&d_th,NULL,get_file_chunks,dthread))
				printf("failed to create thread\n");
		}		



		sleep(1); 
		int serve_fd = create_server_for_peer(retpeer->port,retpeer->ipaddress);  //open for accepting other peers  for connection to download chunks
		open_for_connection(serve_fd,retpeer);
		close(sockfd);
	}
	else //if port entered was invalid
	{
		printf("Enter port\n");
	}
}