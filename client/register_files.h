//register files with server and receive reply for each file
int register_files_with_server(int sockfd, s_peer* peer,s_peer* retpeer,s_download_thread *dthread)
{
	//create struct for file list
	s_file_list * filelist =malloc(sizeof(s_file_list));				
	int i=0,j=0;	
	int if_partial_download=-1;

	//send current peer details to the tracker for registration
	if(write(sockfd , peer, sizeof(s_peer)) < 0)
	{
	    printf("Send failed\n");
	    exit(0);
	}
	
	//read to check if the files were registered and the files which are partially downloaded
	if(read(sockfd,retpeer,sizeof(s_peer))>0);
	//sleep till the next write and the tracker is available for reading
	sleep(1);
	int n = retpeer->no_of_files;
	printf("no_of_files: %d\n",n);
	printf("\n\nFiles you have registered\n");
	
	//print files that are registered
	for(i=0;i<retpeer->no_of_files;i++)
	{
		int flag=0;
		long filesize = retpeer->all_files[i].filesize;
		int chunksize = retpeer->all_files[i].chunk_size;
		int nchunks = (int)ceil((double)filesize/(double)chunksize);

		//check if any file is partially downloaded
		for(j=0;j<nchunks;j++)
		{
			//if partial file found, prepare file for the full download
			if(retpeer->all_files[i].chunks_present[j]!=true)
			{
				flag=1;
				printf("%s: Partially Downloaded\n",retpeer->all_files[i].filename);
				dthread->sockfd = sockfd;
				strcpy(dthread->filename,retpeer->all_files[i].filename);
				dthread->peer = retpeer;
				dthread->filesize = retpeer->all_files[i].filesize;
				if_partial_download = i;
				break;
			}
		}
		//if no partial file available for download, continue asking the user 
		if(flag==0)
		{
			printf("%s: Registered\n", retpeer->all_files[i].filename);
		}
	}
	//return the index of partially downloaded file
	return if_partial_download;
}