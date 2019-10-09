// upload the chunk (return the chunk reply to the peer)
void upload(char* filename, int chunk_number,int sockfd, s_peer * retpeer)
{
	int i=0;
	int actually_read;
	int fptr;

	//finf file in the current peer
	for(i=0;i<retpeer->no_of_files;i++)
	{
		if(strcmp(retpeer->all_files[i].filename,filename)==0)
		{
			uint8_t buff[MAX_BUF];

			//open file for reading
			fptr = open(retpeer->all_files[i].fileaddress,O_RDWR);
	//		printf("LSEEK POSITION: %ld\n",lseek(fptr,chunk_number*retpeer->all_files[i].chunk_size,SEEK_SET));
			//read the chunk from the file
			actually_read = read(fptr, buff, retpeer->all_files[i].chunk_size);
	//		printf("CURRENT BUFF SIZE: %d\n",actually_read);
    		//return the chunk to the peer
    		write(sockfd, buff, actually_read);
    		close(fptr);
		}
	}
}


//receive chunk request to the peer
void * request_from_other_peer(void *arg)
{
	//printf("Thread check\n");
	s_peer_serve * cap = (s_peer_serve*)arg;
	s_chunk_request * request;
	request = malloc(sizeof(s_chunk_request));
	read(cap->connfd,request,sizeof(s_chunk_request));
	//sleep(1);
	upload(request->filename,request->chunk_number,cap->connfd,cap->retpeer);
	close(cap->connfd);
}