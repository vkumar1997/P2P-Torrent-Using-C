//struct for storing files for a peer
typedef struct files
{
	char filename[200];
	int chunk_size;
	char fileaddress[200];
	long filesize;
	bool chunks_present[8192];
} s_files;


//struct for storing peers
typedef struct peer
{
	char ipaddress[40];
	uint16_t port;
	int no_of_files;
	s_files all_files[100];
} s_peer;


//sending file for download to peer and asking for chunks from tracker
typedef struct file_list_reply
{
	char filename[200];
	long filesize;
	bool partial_download;
} s_file_list_reply;


//sending file list for download to peer
typedef struct file_list
{
	int no_of_files;
	s_file_list_reply filelist[200];
} s_file_list;


//chunks in a peer available for download
typedef struct peer_chunks
{
	char ipaddress[40];
	uint16_t port;
	int no_of_chunks;
	int chunks[8192];
} s_peer_chunks;


//peers that have the chunks for a filename (ask tracker)
typedef struct location_reply
{
	int no_of_ipaddress;
	s_peer_chunks all_address[128];
} s_location_reply;


//sending chunk request to a peer
typedef struct chunk_request
{
	char filename[200];
	int port;
	char ipaddress[40];
	int chunk_number;
	char download_address[200];
	int chunk_size;
	long filesize;
	bool status; //if downloaded
} s_chunk_request;


//starting peer as a server for other peers
typedef struct peer_serve
{
	int connfd;
	s_peer *retpeer;
} s_peer_serve;


//starting a different download thread
typedef struct downloadthread
{
	int sockfd; 
	char filename[200]; //filename for download
	s_peer * peer; //peer info
	long filesize;
	int partial_download; //if file was downloaded partially
} s_download_thread;


//update chunk at the tracker
typedef struct chunk_update
{
	char filename[200];
	int chunk_number;
} s_chunk_update;


//same struct for updating chunk or asking for chunks
typedef struct message_type
{
	int type; //0-asking for chunks 1-updating chunks
	s_file_list_reply download_file;
	s_chunk_update update_chunk;
} s_message_type;