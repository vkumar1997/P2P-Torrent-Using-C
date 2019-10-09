#include "location_reply.h"
#include "chunk_update_reply.h"
#include "add_new_peer.h"
#include "find_file_list.h"
#define MAX 128


void * register_peer(void *arg)
{
	//argument for socket id
	int newSocket = *((int *)arg);

	//accept peer from socket
	s_peer *peer = malloc(sizeof(s_peer));
	read(newSocket, peer,sizeof(s_peer));
	sleep(1);

	//check if already existing peer. check for old files. add new files. if not already existing, add new peer. return registration details to clients
	int peer_id=check_already_existing_peer(peer,newSocket);
	s_peer *peer2 = malloc(sizeof(s_peer));
	
	//accept updated peer to return filelist
	read(newSocket, peer2,sizeof(s_peer));

	//return filelist for download if asked for
	find_file_list(peer2,newSocket);
	int counter = 0;
			
	//accept location request or chunk update request in a loop	
	while(true)
	{
		//accept message. check userstruct.h for details
		s_message_type *message = malloc(sizeof(s_message_type));
		if (recv(newSocket, message,sizeof(s_message_type),0)>0)
		{
			//return chunk locations
			if(message->type==0)
			{
				prepare_reply_packet(message,peer2,peer_id,newSocket);	
			}

			//update chunk status and return reply 
			if(message->type==1)
			{
				chunk_status_update(active_connections[peer_id],message->update_chunk.filename,message->update_chunk.chunk_number);
				counter = 0;
			}

		}
		//check if peer has disconnected to free port. contact three times
		else
		{
			counter++;
		}
		if(counter == 3)
		{
			break;
		}
		free(message);
	}
}




void open_for_connection(int sockfd)
{
	//create array for threads
	pthread_t tid[MAX];
	int i = 0;
	struct sockaddr_storage cli;

	//loop for accepting peers
	while(1)
	{
		int len = sizeof(cli);

		//accept peers
		int connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
		if(connfd < 0)
		{
			printf("Server accept failed");
			continue;
		}
		else
		{
			printf("Server accept the client...\n");

			//create new thread if peer found and register the peer
			if(pthread_create(&tid[i++],NULL,register_peer,&connfd) != 0)
				printf("failed to create thread\n");
		}
	}
}

