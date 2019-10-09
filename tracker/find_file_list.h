//removing duplicates from a fileList
int removeDuplicates(char filelist[128][200],char uniquelist[128][200],long filesize[128],long uniquesize[128],int no_of_files) 
{ 
    int i=0,j=0,r=0;
    int flag=0;

    //check for all files
    for(i=0;i<no_of_files;i++)
    {

    	flag=0;
    	//check with each file
    	for(j=i+1;j<no_of_files;j++)
    	{
    		//if duplicate found
    		if(strcmp(filelist[i],filelist[j])==0)
    		{
    			flag=1;
    			break;
    		}
    	}
    	//if duplicate not found
    	if(flag==0)
    	{
    		strcpy(uniquelist[r],filelist[i]);
    		uniquesize[r++]=filesize[i];
    	}	
    }
    //return number of unique files
    return r;
}




//send file list for download
void find_file_list(s_peer* peer, int newSocket)
{
	char filelist[128][200]; //filelist
	char uniquelist[128][200]; //remove all duplicates (happens when multiple peers have same files for download)
	long filesize[128],uniquesize[128]; //for storing filesize

	int i,j,k;
	int no_of_files = 0;
	s_file_list* files; //for returning file list
	files = malloc(sizeof(s_file_list));
	files->no_of_files=0;

	//find all files from peers one by one and store it to filelist
	for(i=0;i<no_of_connections;i++)
	{
		for(j=0; j < active_connections[i]->no_of_files; j++)
		{
			//check if file is already present in the peer
			int flag=0;
			for(k=0;k<peer->no_of_files;k++)
			{
				if(strcmp(active_connections[i]->all_files[j].filename,peer->all_files[k].filename)==0)
				{
					flag=1;
					break;
				}
			}
			//if file not present, add to fileList
			if(flag==0)
			{
				strcpy(filelist[no_of_files],active_connections[i]->all_files[j].filename);
				filesize[no_of_files++]=active_connections[i]->all_files[j].filesize;
			}
		}
	}

	//remove duplicates from fileList
	no_of_files = removeDuplicates(filelist,uniquelist,filesize,uniquesize,no_of_files);

	//adding to struct to return to peer
	for(i = 0; i<no_of_files;i++)
	{
		strcpy(files->filelist[files->no_of_files].filename,uniquelist[i]);
		files->filelist[files->no_of_files++].filesize = uniquesize[i];
	}

	sleep(1); //wait till peer comes to read. write is non-blocking
	//printf("Check: %d\n", files->no_of_files);
	write(newSocket,files,sizeof(s_file_list)); //return filelist to peer
}