//update chunk at a peer for a filename
void chunk_status_update(s_peer * peer,char* filename, int chunknumber)
{
	int i=0;
	
	for(i=0;i<peer->no_of_files;i++)
	{
		if(strcmp(peer->all_files[i].filename,filename)==0)
		{
			//printf("Chunk Updated\n");
			peer->all_files[i].chunks_present[chunknumber] = true;
		}	
	}
}