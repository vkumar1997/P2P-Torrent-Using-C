#define MAX 128 //max no of peers allowed
#define PORT 8040 //port number at which tracker connects


//create tcp socket
int create_sock()
{
	int iSetOption = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);		//0 for tcp
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
	if(sockfd==-1)
	{
		printf("Socket creation failed...\n");
		exit(0);
	}	

	else
	{
		printf("Socket created successfully.\n");
	}
	return sockfd;
}


//bind to socket
void sock_bind(int sockfd)	
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	int bindbool = bind(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr));
	if(bindbool!=0)
	{
		printf("socket bind failed");
		exit(0);
	}
	else
	{
		printf("socket binded successfully\n");
	}

}

//listen to socket with MAX number of connections possible
void sock_listen(int sockfd)
{
	int listenbool = listen(sockfd,MAX);
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