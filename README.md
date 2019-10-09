# P2P-Torrent-Using-C
A minimalist  BitTorrent implementation using C


# Setup

The coding was done in C (Ubuntu 18.04). The sockets used in the program use TCP (relaible and connection oriented) for communication. The protocol used is TCP/IP which is the default protocol for sockets in C.

# Messages Used
The program is a minimalist design of transferring files using peer-to-peer connections and a tracker (to store metadata anout peers). For this purpose, several types of messages were used to communicate between peers and the tracker. The broad categories of messages used are discussed below.

## Register Request
Initially, the peer sends a register request to the tracker. This is implemented using structure in C. The structure contains metadata about the peer to be sent and to be stored in the tracker. The structure contains ipaddress and port of the peer using which other peers can connect, no of files that the peer wants to register, filenames and their filesizes. The structure also contains chunk size for each chunk of the files the peer wishes to register. All the metadata is encapsulated in the structure and sent to tracker using Sockets.


## Register Reply
Once the tracker receives the register request, it stores the metadata received from the peer (volatile storage). Apart from that, the tracker tries to determine whether the peer has ever connected before with the tracker. If the peer had indeed connected with the tracker before, it merges the previous files with the newly registerd files. Furthermore, it determines which files are partially downloaded by the peer (missing chunks). All this information is stored in a structure and returned to the peer.

## File List Request
Once the files are registered, the peer requests for the list of all the files that the peer can download. If the peer has any partially downloaded file (as returned by the tracker, the peer skips this step and moves to just downloading the partial file completely.)

## File List Reply
When the tracker recieves a file list request, it retrieves a list of all the files from the other peers (excluding the files that the requesting peer already has). The tracker ensures that the list is unique i.e., even if multiple peers have the copy of the same file, the file list will only have uniques names. Furthermore, the File List Reply also contains the filesize of each file.

## Location Request
A peer can choose to download a file or just serve other peers with the registered files (seeding). If it chhoses to download a file from the provided file list, it will request the tracker for the possible locations of the chunks for that file. Once ,it receives a reply from the tracker, it will start communicating with those peers and start downloding those chunks. In order to update the locations of the chunk, the peer sends the Location Request to the tracker every once in a while.

## Location Reply
When the tracker receives a location request from the peer, it will find all the peers that contain a copy of the file. It will reply to the requesting peer with a structure encapsulated with the ipaddress and ports of other peers. Furthermore, it will encapsulate the number of chunks each peer has and the chunk number (for each chunk), assuming the chunk size is fixed.

## Chunk Request
In a peer-to-peer file transfer system, peers communicate with each other to transfer files to each other. In such a system, one peer becomes a server while the requesting  peer becomes a client to perform the handshake and transfer the chunk. For requesting a chunk, a peer sends the filename and the chunk number to other peers ( list sent by tracker using location reply).

## Chunk Reply
If a peer receives a chunk request containing the filename and the chunk number, it will check whether the peer already has the file in the file list. Afterward, it will check if it has the chunk requested by the client. Then it will transfer the chunk to the requesting peer. Once, the requesting peer receives the chunk, it will check whether the bytes sent by the serving peer are equal to the bytes requested. If true, it will store the chunk in the respective file in the downloads folder and update the metadata locally. Then, it wil request the tracker to make it a source of that chunk. In Figure 3, you can see that chunk 3 is requested by the peer from other peer at port 8200.

## Chunk Register Request
For registering a chunk, the peer sends the chunk number and the filename to the tracker (so that it can become a source for other peers for that chunk). It does so immediately after downloading the chunk in the same thread.

## Chunk Register Reply
Once the chunk registration is complete at the tracker, it sends a reply to the peer (the coontents of the reply does not matter). If the peer fails to receive a reply from the tracker, it will update the metadata for that chunk locally (not-downloaded status) and will download the chunk again from its group of peers.

# Structures Used
For communication using the TCP/IP protocol, the program uses a lot of structures for encapsulating information and sending to tracker/peer. The major structures facilitating communication are described below.

## files
Contains filename, fileaddress, filesize, chunksize. The struture can be used by the peer/ tracker to store which chunk a peer already has (usng boolean values).

## peer
Contains the ipaddress, port and the number of files for each peer. Also encapsulates an array of files structure mentioned in 3.1.

## file list reply
Contains filename, filesize and if any file is partially downloaded. Sent from tracker to a peer after receiving a file list request. Also, is used to request locations of a chunk of a file that the peer wishes to download (sent from peer to tracker).

## file list
Contains an array of file list reply mentioned in 3.3

## peer chunks
Used for storing information of each peer during a location request. Contains the ipaddress, port, number of chunks and the available chunk numbers for a single peer.

## location reply
Used for sending the locations of chunks for a filename (sent from tracker to peer). Contains number of peers and an array of peer chunks structure mentioned in 3.5.

## chunk request
Used for requesting a chunk from another peer. This structure has two purposes. First, it is used to start a different thread for downloading a chunk. Second, it is used to request that chunk from another peer. Contains metadata like port and ipaddress of the source, download address, filename, chunk number, chunk size, filesize and download status.

## chunk update
This is sent from a peer to the tracker. It contains the filename and the chunk number which is to be registered with the tracker to become a source of that chunk.

Figure 5 and Figure 6 show how the communication between peer-peer and peer-tracker is designed to happen. In Figure 5 , we have most of the initial conversation in a single thread (which involves initial handshake, peer registration and file list request). Afterwards, the communication takes place in two seperate threads. The peer requests the server for locations of chunks once in a while (please see the sendchunks.h header file for more details on how the peer decides when to request). The peer also starts a download thread for every chunks and sends a update request to the tracker once the chunk is downloaded.

In peer-peer communication, there is only one type of message passing which involves chunk transfer. Ofcourse, Figure 5 does not show that a peer can connect to multiple peers simultaneously to download the file. However the program is designed tp connect to as many peers as possible while downloading the file.

# Other Features
## Multiple Connections
A peer can connect to multiple peers at the same time (apart from connecting to tracker). Figure 6 shows that the peer can maintain multiple connections at the same time (it is downloading chunks from three peers and updating the chunk info at the tracker at the same time).



## Parallel Downloading
As shown in Figure 6, a peer can download multiple chunks of the file from multiple peers simultaneously.

## Rarest Chunk Selection
If a peer a source of mutiple chunks, the requesting peer selects the rarest chunk that the source peer has to offer. First the requesting peer calculates the frequency of each chunk available from its peer list. Then for each peer in the peer list, it selects the chunk that has the smallest frequency of all the chunks available. If two or more chunks have the same frequency, it will download the chunk with the highest chunk number. (see Figure 6)

## Chunk Download Completion
As seen in Figure 6, a peer registers the chunk with the tracker once it is done downloading that chunk.

## Failure Tolerace
\subsubsection{Peer Failure while downloading}
If a peer crashes while downloading, it can resume downloading by connecting to the tracker using the same port again. In Figure 7, you can see a peer resuming download after crashing during the initial attempt.


### Peer Failure while uploading
If a peer crashes while uploading, the requesting peer will try to choose other peers for downloading chunks. If there are no other peers available, it will wait for the crashed peer to resume uploading again.

### Tracker failure
Unfortunately, there is no system in place to support tracker failure tolerance right now. This will require the tracker to maintain a database of connected peers, but right now the tracker works sung volatile memory.

# Interface
 The user provides two arguments while starting an instance of the peer code. The first argument is the port number using which the other peers will connect to this peer (this port is used for uploading). The other argument is the fileaddress or the directory address which needs to be registered with the tracker. If a user wishes to uplaod multiple files, it can put them in the directory and provide the directory address as the second argument.
 
 The users are asked whether they want to download any files. If they reply in affirmative, they are shown a list of files from which user can choose one file at a time.
 
 The users are shown the status of active download as seen in Figure 6.
 
# Limitations/Assumptions

	Tracker Failure Tolerance: The tracker does not have any failure tolerance. If the tracker fails, the peer may some still continue to download the chunks from the formed peer-set. But, all the peers can also crash unexpectedly.
	
	Mutilpe File Download: Even though the program supports multiple chunk download simultaneoulsly as requested in the problem statement, it is not suited to download multiple files at a time. A user may end the current program and connect using the same port to download another file.
	
	Fixed Localhost Address: The program was developed using local host and currently all the peers use a hard-coded IP address i.e., 127.0.0.1. However, if a peer wishes to connect using a different ipaddress, it may do so by changing the hard-coded ip address value and compiling the program again.
