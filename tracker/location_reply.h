#include <math.h>
#include "constants.h"

//return locations for chunks
void loc_reply(char* filename,s_location_reply *lcreply,s_peer *peer)
{
	int i=0,j=0,k=0;

	//for each connection
	for (i=0;i<no_of_connections;i++)
	{
		//check if the connection is not the same peer
		if(strcmp(peer->ipaddress,active_connections[i]->ipaddress)!=0 || peer->port!=active_connections[i]->port)
		{
			//for each file in each connection
			for (j=0;j<active_connections[i]->no_of_files;j++)
			{			
				//find the file for each connection
				if(strcmp(filename,active_connections[i]->all_files[j].filename)==0)
				{
					strcpy(lcreply->all_address[lcreply->no_of_ipaddress].ipaddress, active_connections[i]->ipaddress); //store the ipaddress
					lcreply->all_address[lcreply->no_of_ipaddress].port = active_connections[i]->port; //store the port
					int n_chunks = (int)(ceil((double)active_connections[i]->all_files[j].filesize / (double)active_connections[i]->all_files[j].chunk_size)); //find number of chunks
					lcreply->all_address[lcreply->no_of_ipaddress].no_of_chunks =0;
					for(k=0;k<n_chunks;k++)//for each chunk
					{
						//check if chunk present in that peer for that filename
						if(active_connections[i]->all_files[j].chunks_present[k]==true)
						{
							//store that the chunk is present in that peer for that filename
							lcreply -> all_address[lcreply->no_of_ipaddress].chunks[lcreply -> all_address[lcreply->no_of_ipaddress].no_of_chunks++] = k;
						}
					}
					//update number of peers
					lcreply->no_of_ipaddress++;
					break;
				}
			}
		}
	}
}




//add filename if filename was not already present in peer
void add_filename(s_peer* mypeer,int peer_id, char* filename,long filesize)
{
	int nchunks,i; 
	char fileaddress[200] = "downloads/";
	int flag = 1;

	//check if file already present in the peer
	for(i=0;i<mypeer->no_of_files;i++)
	{
		if(strcmp(mypeer->all_files[i].filename,filename)==0)
		{
			flag = -1;
			break;
		}
	}

	//if not, add file
	if(flag!=-1)
	{
		//if file is less than 10mb
		if(filesize<10*1000*1000)
		{
			nchunks = 10; //divide in 10 chunks
			mypeer -> all_files[mypeer -> no_of_files].chunk_size = filesize/nchunks; //store chunk size
			mypeer -> all_files[mypeer -> no_of_files].filesize = filesize; //store filesize
		}
		
		//if file is greater than 10mb
		else
		{
			mypeer -> all_files[mypeer -> no_of_files].filesize = filesize; //store filesize
			mypeer -> all_files[mypeer -> no_of_files].chunk_size = 1000*1000; //store chunksize
		}

		nchunks = (int)(ceil((double)mypeer -> all_files[mypeer -> no_of_files].filesize/ (double)mypeer -> all_files[mypeer -> no_of_files].chunk_size));//calculate number of chunks
		
		//update download status for each file
		for(i=0;i<nchunks;i++)
		{
			mypeer -> all_files[mypeer -> no_of_files].chunks_present[i] = false; 	
		}

		//update filename for the file
		strcpy(mypeer->all_files[mypeer->no_of_files].filename,filename);	
		strcat(fileaddress,filename);
		
		//update download address for the file
		strcpy(mypeer->all_files[mypeer->no_of_files].fileaddress,fileaddress);
		
		//update no of files
		mypeer->no_of_files++;
		
		//update active connections
		active_connections[peer_id]=mypeer;
	}
}


//send chunk locations for a filename to a peer
void prepare_reply_packet(s_message_type * message, s_peer * peer, int peer_id,int newSocket)
{
	//prepare packet for sending chunks to a peer
	s_location_reply *lcreply; //check userstruct.h
	char filename[200]; 
	long filesize;

	lcreply = malloc(sizeof(s_location_reply));
	lcreply->no_of_ipaddress = 0;
	strcpy(filename,message->download_file.filename);
	filesize = message->download_file.filesize;
	
	//find chunk locations
	loc_reply(filename,lcreply,peer);
	sleep(1);//sleep till peer reaches read. to reduce tcp errors
	
	//send chunk locations
	write(newSocket,lcreply,sizeof(s_location_reply));

	//check if file was already partially downloaded
	if(!message->download_file.partial_download)
	{
		//if not, create new file for peer
		add_filename(active_connections[peer_id],peer_id,filename,filesize);
	}
	free(lcreply);
}