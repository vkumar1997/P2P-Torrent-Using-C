//create new socket for client
int createsocket()
{
	int iSetOption = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); //0 for TCP protocol
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));	
	if(sockfd==-1)
	{
		printf("\nSocket creation failed...\n");
		return -1;
	}	
	
	else
	{
		//printf("\n\nSocket created successfully.\n");
		return sockfd;
	}
	return sockfd;
}


//connect to tcp. IP and Port stored in servaddr
int connectsocket(int sockfd,struct sockaddr_in servaddr)
{	
	int connectbool = connect(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr));
	if(connectbool != 0)
	{
		printf("connection failed\n");
		return -1;
	}
	else
	{
		//printf("established connection\n");
		return 1;
	}
}