//check if chunk is downloaded
bool check(s_peer* retpeer, int k,int index)
{
	return retpeer->all_files[index].chunks_present[k];
}

//remove a chunk from all peers (after downloading chunk)
int remove_match(int array[], int length, int match)
{

    size_t shift =0;
    int removed =0;

    //find element in the array
    for (size_t i = 0; i < length; ++i)
    {
        if (array[i] == match)
        {
            removed++;
        }

        else
        {
            array[shift++] = array[i];
        }
    }
    return (length - removed);
}


//check if file is downloaded. print file download status
bool check_if_downloaded(s_peer * peer, int index)
{
	int j=0;
	bool flag = true;

	long filesize = peer->all_files[index].filesize;
	int chunksize = peer->all_files[index].chunk_size;
	int nchunks = (int)ceil((double)filesize/(double)chunksize);

	//check for each chunk
	for(j=0;j<nchunks;j++)
	{

		//chunk not downloaded
		if(peer->all_files[index].chunks_present[j]!=true)
		{
			flag=false;
			printf(".");
		}

		//chunk downloaded
		else
		{
			printf("|");	
		}
	}	
	printf("\n");

	//downloaded file
	if(flag==true)
	{
		printf("%s: Downloaded\n", peer->all_files[index].filename);
	}
	return flag;
}


//frequency of a chunk in all peers
int frequency(int a[12000], int n, int x) 
{ 
    int count = 0; 
    for (int i=0; i < n; i++) 
       if (a[i] == x)  
          count++; 
    return count; 
} 
