#define MAX_ONS 2000
#define MAX_BUF 1000000
#define IP_ADDRESS "127.0.0.1"

#include "chunk_reply.h"

//create socket as a tracker
int create_sock()
{
	int iSetOption = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);		
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
	if(sockfd==-1)
	{
		printf("Socket creation failed...\n");
		return -1;
	}	

	else
	{
		printf("Socket created successfully.\n");
		return sockfd;
	}
}


//bind to socket
int sock_bind(int sockfd,int port, char* ipaddress)
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	servaddr.sin_port = htons(port);
	int bindbool = bind(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr));
	if(bindbool!=0)
	{
		printf("socket bind failed\n");
		return -1;
	}
	else
	{
		printf("socket binded successfully\n");
		return 1;
	}

}

//listen to socket with max number of peers allowed at one time (MAX_ONS)
void sock_listen(int sockfd)
{
	int listenbool = listen(sockfd,MAX_ONS);
	if(listenbool != 0)
	{
		printf("listen failed");
		exit(0);
	}
	else
	{
		printf("Server listening\n");
	}
}


//create server for other peers to serve chunks of a file
int create_server_for_peer(int port, char* ipaddress)
{
	int sockfd = create_sock();
	sock_bind(sockfd,port,ipaddress);
	sock_listen(sockfd);
	return sockfd;
}


//create multiple connections to accept multiple clients
void open_for_connection(int sockfd,s_peer *retpeer)
{
	//array for multiple clients
	pthread_t tid[MAX_ONS];
	int i = 0;
	struct sockaddr_storage cli;
	s_peer_serve *cap = malloc(sizeof(s_peer_serve));

	while(1)
	{
		int len = sizeof(cli);
		//accept a new client
		int connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
		if(connfd < 0)
		{
			printf("Server accept failed\n");
			continue;
		}
		else
		{
			cap -> connfd = connfd;
			cap ->	retpeer = retpeer;
			//create new thread for user
			if(pthread_create(&tid[i],NULL,request_from_other_peer,cap) != 0)
				printf("failed to create thread\n");
			
			pthread_join(tid[i], NULL);
			i=i+1;
		}
		//restart array after serving 100 chunks
		if(i%100==0)
		{
			i=0;
		}

	}
}