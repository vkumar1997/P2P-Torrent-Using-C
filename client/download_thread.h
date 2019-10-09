#define MAX_BUFF 1000000
s_download_thread *dthread;

//download chunk
int download(int peer_id,s_chunk_request* request)
{
	int current_size = 0;
	uint8_t buff[MAX_BUFF];
//	printf("%d\n", request->port);

	//find size of chunk
	if(request->chunk_number == (int)(ceil((double)request->filesize/(double)request->chunk_size))-1)
	{
		current_size = request->filesize % request->chunk_size;
	}
	else
	{
		current_size = request->chunk_size;
	}

	//printf("Chunk Size requested: %d\n",current_size);
	//request chunk from other peer
	if(!write(peer_id,request,sizeof(s_chunk_request))>0)
	{
		return -1;
	}

	//read chunk from other peer
	long bytes = read(peer_id,buff,current_size);
	
	//check if chunk size is as expected
	if(bytes!=current_size)
		return -1;

	//printf("%ld\n", bytes);

	//save chunk to file
	pthread_rwlock_wrlock(&rwlock);
	int fptr;
	fptr = open(request->download_address,O_RDWR | O_CREAT, 0666);
	lseek(fptr,request->chunk_number*request->chunk_size,SEEK_CUR);
	write(fptr, buff,current_size);
	close(fptr);
	pthread_rwlock_unlock(&rwlock);
	return 1;
}



//download thread
void * download_chunk(void * args)
{
	int status;
	struct sockaddr_in servaddr;
	s_chunk_request * request = (s_chunk_request*)args;
	int peer_id = createsocket();
	
	//create socket
	if(peer_id<0)
	{
		request->status=false;
		pthread_exit((void*)request);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(request->ipaddress);
	servaddr.sin_port = htons(request->port);

	//connect to another peer's socket
	if(connectsocket(peer_id,servaddr)<0)
	{
		request->status=false;
		pthread_exit((void*)request);
	}
	
	//download chunk
	if(download(peer_id,request)==1)
	{
		request->status=true;
		pthread_exit((void*)request);
	}

	//if download fails,return failed status
	else
	{
		request->status=false;
		pthread_exit((void*)request);
	}
}



