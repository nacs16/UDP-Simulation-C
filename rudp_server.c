/* Jacob Weimer -- jaweimer
 * Nicholas Solano -- nsolano
 * Created: 11-3-14
 * Last Edited: 11-9-14
 * Assignment 5 P438 
 
 * Reliable UDP Server that receives text from client and writes it
 * To a text file called Copy.txt

 *README
 Please run the server ./rudp_server
 Then run the client: ./rudp silo 16100 test.txt
 test.txt is textfile with message to send and copy.
 If this portnumber is unavailable please chang portno below
 */

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


// the main of the program takes no inputs
int main()
{
  int socketA, nBytes;
  struct sockaddr_in servaddr, cliaddr;
  struct sockaddr_storage servStorage;
  socklen_t addr_size, client_addr_size;
  int i;

  //change this if the port is being used
  int portno = 16100;
  
  //Create the sockets for the server
  // Sock_DGRAM means this is UDP server
  socketA = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketA == -1)
    {
      printf("Socket could not be created");
    }
  
  //initialize socket structure
  bzero((char *) &servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(portno);
  memset(servaddr.sin_zero, '\0', sizeof servaddr.sin_zero);
  
  char new_file[] = "copied.txt";
  FILE *fp;

  //bind the socket to the address and portno
  if (bind(socketA, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
      printf("Failed to Bind\n");
    }
  
  int file_size;
  recvfrom(socketA,&file_size,sizeof(int),0,(struct sockaddr *)&servStorage, &addr_size);
  printf("FILE SIZE: %d\n", file_size);
  
  addr_size = sizeof servStorage;
  
  size_t total_size = 0;
  
  int window = 100;
  if (file_size < 100)
  {
    window = file_size;
  } 
  
  char seq[20];
  int sequence;  
  int old_seq = 0;
  while(total_size < file_size)
    {
      
	if ((file_size - total_size) < window)
	{
	window = file_size - total_size;
	}
	  
	  printf("\nWaiting for data... \n");
      
      char buffer[window];
	  
      //receive sequence
	  recvfrom(socketA, seq, 20,0,(struct sockaddr *)&servStorage, &addr_size);
	  sequence = atoi(seq);	  
	  if (sequence == old_seq + 1)
	  {
		//receive text
	    recvfrom(socketA, buffer, window,0,(struct sockaddr *)&servStorage, &addr_size);
      
        printf(buffer);
      
        char ack[1];
	    ack[0] = '1';
        sendto(socketA, ack, sizeof(int), 0,(struct sockaddr *)&servStorage, addr_size); 
        fp = fopen(new_file,"a+");
        if(fwrite(buffer,1,sizeof(buffer),fp)<0)
	    {
	      printf("error writing file\n");
	      exit(1);
	    }
	  
	    total_size = total_size + window;
        window = window + 100;
		old_seq = sequence;
		
		//debugging printfs 
		//printf("total_size = %d\n", total_size);
	    //printf("file_size = %d\n", file_size);
	    //printf("window size = %d\n", window);
		//printf("Sequence = %d", sequence);
	  }
    }
  if (total_size == file_size)
  {
    printf("\nComplete!\n");
  }
  close(socketA);
  return(0);
}
