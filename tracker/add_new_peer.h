//add new peer to the peer list
void add_new_peer(s_peer* peer)
{
	active_connections[no_of_connections++]=peer;
}


//find existing files
void merge_with_existing_files(s_peer* peer, int i)
{
	int j=0,k=0;
	int new_files = peer->no_of_files;
	//printf("Checking files\n");
	//printf("New Files: %d\n",new_files);
	
	//check for each file if it is already in the existing file list
	for(j=0;j<new_files;j++)
	{
		//ckeck each file from the old peer
		int old_files = active_connections[i]->no_of_files,existing_file=0;
		for(k=0;k<old_files;k++)
		{
			if(strcmp(peer->all_files[j].filename,active_connections[i]->all_files[k].filename)==0)
			{
				printf("File found\n");
				existing_file = 1;
				break;
			}	
		}

		//if file not found. add new file
		if(existing_file == 0)
		{
			//printf("File not found. Adding file\n");
			int n=active_connections[i]->no_of_files;
			strcpy(active_connections[i]->all_files[n].filename, peer->all_files[j].filename);
			active_connections[i]->all_files[n].chunk_size= peer->all_files[j].chunk_size;
			strcpy(active_connections[i]->all_files[n].fileaddress,peer->all_files[j].fileaddress);
			active_connections[i]->all_files[n].filesize = peer->all_files[j].filesize;

			int nchunks = (int)ceil((double)peer->all_files[j].filesize/(double)peer->all_files[j].chunk_size );
			for (k=0;k <nchunks;k++)
			{
				active_connections[i]->all_files[n].chunks_present[k] = peer->all_files[j].chunks_present[k];
			}
			active_connections[i]->no_of_files+=1;
		}	
		
	}
}



//check if the peer already existed
int check_already_existing_peer(s_peer* peer,int newSocket)
{
	int i=0,flag=0;
	printf("Check existing user\n");
	//check each peer using ipaddress and port
	for(i=0;i<no_of_connections;i++)
	{
		//printf("Port number: %d\n",active_connections[i]->port);
		if(strcmp(peer->ipaddress,active_connections[i]->ipaddress)==0 && peer->port==active_connections[i]->port)
		{
			//printf("user found\n");
			flag=1;

			//if peer found. check existing files and merge with new file list
			merge_with_existing_files(peer,i);
			break;			
		}
	}

	//if peer npt found, create new peer
	if(flag==0)
	{
		printf("Creating new user\n");
		add_new_peer(peer);
	}

	printf("Total files: %d\n",active_connections[i]->no_of_files);
	printf("No of conn: %d\n", no_of_connections);
	
	//confirm the registration to the peer
	write(newSocket,active_connections[i],sizeof(s_peer));
	return i;
}