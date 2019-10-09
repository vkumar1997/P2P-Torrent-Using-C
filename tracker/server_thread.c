#include <unistd.h> //for null pointer
#include <stdio.h>  //standard input and putput operations
#include <sys/socket.h>  //standard socket operations like bind, accept connect
#include <stdlib.h>  // standard c library
#include <netinet/in.h>  //for socket address
#include <string.h> //for string operations
#include <stdbool.h> //for boolean

#include <arpa/inet.h> //for ip address
#include <fcntl.h> //file operations
#include<pthread.h> //multiple threading

#include "userstruct.h" //user defined structures
#include "socket.h" //for creating socket connection
#include "register.h" //register and communicating with peer





int main()
{	
	int sockfd=create_sock(); //create tcp channel
	sock_bind(sockfd); //bind socket
	sock_listen(sockfd); //listen to socket
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	open_for_connection(sockfd); //open for accepting clients and talk to clients
	close(sockfd); //close channel
}

