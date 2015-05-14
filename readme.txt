In 2 putty windows:
Server: Chosen port 16100, if unavailable, change line 39 in rudp_server.c to an open port# 
gcc -o rudp_server rudp_server.c
	$ ./server
Client: takes 3 command line arguments: host, port, text filename
	$ gcc -o rudp rudp.c
	$ ./rudp silo 16100 filename.txt 

Client takes as a command line argument the name of a txt file to send to server.
Begins by sending the size of the file to server.
Client first sends packet of 100 bytes to server along with header information containing sequence#
Server and Client simultaneously compute the appropriate window buffer size, increasing by 100 each sequence.
Server appends whatever text it receives to Copied.txt
Server closes when complete or error when writing to Copied.txt
Client closes if fails to send packet after 5 attempts or sending completes.