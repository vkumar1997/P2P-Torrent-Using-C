//Add new file for download
int add_filename(s_peer* mypeer, char* filename,long filesize)
{
	int nchunks,i;
	//fileaddress for download
	
	char fileaddress[200] = "downloads/";
	//filesize less than 10mb
	
	if(filesize<10*1000*1000)
	{
		nchunks = 10;
		mypeer -> all_files[mypeer -> no_of_files].chunk_size = filesize/nchunks;
		mypeer -> all_files[mypeer -> no_of_files].filesize = filesize;
		
	}
	
	//filesize greater than 10mb
	else
	{
		mypeer -> all_files[mypeer -> no_of_files].filesize = filesize;
		mypeer -> all_files[mypeer -> no_of_files].chunk_size = 1000*1000;
	}
	
	//number of chunks
	nchunks = (int)(ceil((double)(mypeer -> all_files[mypeer -> no_of_files].filesize) / (double)(mypeer -> all_files[mypeer -> no_of_files].chunk_size)));
	
	//initially no chunks present
	for(i=0;i<nchunks;i++)
	{
		mypeer -> all_files[mypeer -> no_of_files].chunks_present[i] = false; 	
	}	
	
	strcat(fileaddress,filename);
	//fileaddress
	strcpy(mypeer->all_files[mypeer->no_of_files].fileaddress,fileaddress);
	
	//filename
	strcpy(mypeer->all_files[mypeer->no_of_files].filename,filename);
	
	//update number of files
	return mypeer->no_of_files++;
}
