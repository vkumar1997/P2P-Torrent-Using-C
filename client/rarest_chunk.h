//no of parallel chunk downloading allowed
#define MAX_PEERS 200

bool rarest_chunk(s_location_reply *reply, s_peer* retpeer,int index, char * filename, int sockfd)
{
	//no of parallel chunk downloading allowed
	pthread_t tid[MAX_PEERS];
	int no_of_threads = 0;
	int i=0,j=0,no_of_found_chunks=200,smallest,smallest_index;
	int arr[12000];
	void *status;

	//check if file is already downloaded
	bool check_for_download = check_if_downloaded(retpeer, index);
	
	//start from first or last in the peer list (for failure tolreance)
	bool rndn = rand() & 1;
	
	//if file not downloaded
	if(!check_for_download)
	{
		//for each peer that has a copy of the file
		for(i=0;i<reply->no_of_ipaddress;i++)
		{
			//for each chunk that is present in the peer
			for(j=0;j<reply->all_address[i].no_of_chunks;j++)
			{
				//check if chunk is already doenloaded in the peer
				if(check(retpeer,reply->all_address[i].chunks[j],index)==false)
				{
					//store all chunks from all peers in an array
					arr[no_of_found_chunks++] = reply->all_address[i].chunks[j]; 
				}
			}
		}
		

		int r = 0;
		//find rarest chunk for each peer
		for(i=0;i<reply->no_of_ipaddress;i++)
		{
			if(rndn==1)
			{
				r=reply->no_of_ipaddress-1-i;
			}
			else
			{
				r=i;
			}

			//counter for frequency of a chunk
			smallest = no_of_found_chunks;

			//index of the rarest chunk
			smallest_index = -1;

			//for each chunk that is present in the peer
			for(j=0;j<reply->all_address[r].no_of_chunks;j++)
			{
				//frequency of a chunk in all the peers
				int count = frequency(arr,no_of_found_chunks,reply->all_address[r].chunks[j]);
				
				//finding rarest chunk
				if(count<=smallest && count!=0)
				{
					smallest=count;
					smallest_index = reply->all_address[r].chunks[j];
				}
			}

			//index of the rarest chunk
			if(smallest_index!=-1)
			{
				//request for rarest chunk
				s_chunk_request * request;
				request = malloc(sizeof(s_chunk_request)); 
				request->port = reply->all_address[r].port; //port
				strcpy(request->ipaddress,reply->all_address[r].ipaddress); //ipaddress
				request->chunk_number = smallest_index; //index of rarest chunk
				strcpy(request->filename,filename); //filename
				strcpy(request->download_address,retpeer->all_files[index].fileaddress); //fileaddress
				request->chunk_size = retpeer->all_files[index].chunk_size; //chunk size
				request->filesize = retpeer->all_files[index].filesize; //filesize
				request->status =false; //status of downloading
				
				//assign chunk to this peer and remove from other peers
				no_of_found_chunks = remove_match(arr,no_of_found_chunks,smallest_index);
				printf("Downloading chunk %d from ipaddress: %s and port: %d\n", smallest_index,reply->all_address[r].ipaddress,reply->all_address[r].port);
				


				//start a thread for downloading
				if(pthread_create(&tid[no_of_threads++],NULL,download_chunk,request))
					printf("failed to create thread\n");
			}
		}

		//update download status at peer as well as tracker
		for (i = 0; i < no_of_threads; i++)
		{
			s_chunk_request *req;
	    	pthread_join(tid[i], (void*)&req); //wait for thread to download

	    	//if chunk was downloaded
	   		if(req->status==true)
	   		{
	   			s_message_type *update = malloc(sizeof(s_message_type)); 
	   			update->type=1;
				strcpy(update->update_chunk.filename,filename);
				update->update_chunk.chunk_number = req->chunk_number;
	   			retpeer->all_files[index].chunks_present[req->chunk_number] = true; //update status at peer
	   			send(sockfd,update,sizeof(s_message_type),0); //send update to tracker
	   			printf("Chunk Updated: %d\n",req->chunk_number);
	   		}
	   	}

	    return false; //if file is not downloaded
	}
	else
	{
		return true; //if file is downloaded
	}
}