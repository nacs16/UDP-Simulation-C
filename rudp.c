/* Jacob Weimer -- jaweimer
 * Nicholas Solano -- nsolano
 * Assignment 5 P438 
 * Created: 11-3-14
 * Last Edited: 11-9-14
 * Implementing a 'reliable' UDP program
 * Please enter './rudp silo 16100' for input
 * On another prompt please put ./rudp_server
 * 
 */


//header declarations
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

//main takes 'silo', port number, and filename as arguments
int main(int argc, char *argv[])
{

  // declare sockets, ports, and strings
  int sockfd, portno, n, nBytes;
  char recvack[1];
  struct sockaddr_in servaddr;
  struct hostent *server;
  socklen_t addr_size;
  char buff[2000];


  //convert hostname and portnumber and file
  portno = atoi(argv[2]);
  server = gethostbyname(argv[1]);
  char *read_file = argv[3];

  //create the scoket and start making servaddr
  sockfd=socket(AF_INET,SOCK_DGRAM,0);
  bzero(&servaddr, sizeof servaddr);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port=htons(portno);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(servaddr.sin_zero, '\0', sizeof servaddr.sin_zero);
  addr_size = sizeof servaddr;
  
  //open file to make sure it exists and get the file size
  FILE *fp;
  fp=fopen(read_file,"r");
  if(fp==NULL)
    {
      printf("file does not exist\n");
    }
  fseek(fp,0,SEEK_END);
  size_t file_size=ftell(fp);
  fseek(fp,0,SEEK_SET);
  
  
  size_t total_size = 0;
  int window = 100;
  if (file_size < 100)
  {
    window = file_size;
  }  
  sendto(sockfd, &file_size, sizeof(size_t), 0,(struct sockaddr *)&servaddr, addr_size);
  
  int sequence = 1;  
  
  while(total_size < file_size)
    {
      if ((file_size - total_size) < window)
	{
	  window = file_size - total_size;
	}
      
      fseek(fp, total_size, SEEK_SET);
      char file_buffer[window];
      if(fread(file_buffer,window,1,fp)<=0)
	{
	  printf("unable to copy file into buffer\n");
	  exit(1);
	}
      printf(" %s\n ",&file_buffer);
      int i = 1;
      int checknum = 0;
      // this while loop will run and try to send the string up to 5 times
      while(checknum == 0 && i <=5)
	  { 
		printf("Attempt %d\n", i);
	    //send sequence
	    char seq[20];
	    sprintf(seq, "%d", sequence);
	    sendto(sockfd, seq, 20, 0,(struct sockaddr *)&servaddr, addr_size);
	  
	    sendto(sockfd, file_buffer, window, 0,(struct sockaddr *)&servaddr, addr_size);
	    checknum = recvfrom(sockfd, &recvack, 1024, 0, NULL,NULL);
	    if(checknum)
	      {
		    printf("Received Ack\n");
		    total_size = total_size + window;
		    window = window + 100;
			sequence++;
	        
			//debugging prints
			//printf("total_size = %d\n", total_size);
	        //printf("file_size = %d\n", file_size);
	        //printf("window size = %d\n", window);
	      }
	    i++;	
	  }	  
    }
  return(0);
  close(sockfd);
}



