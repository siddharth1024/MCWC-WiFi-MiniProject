#include <stdio.h>
#include <string.h>
#include <unistd.h> // read(sockfd, fname, 256);
#include <arpa/inet.h> // inet_xxx, ntohs

int main(int argc, char *argv[]) {

  int sockfd = 0;
  int bytesReceived = 0;
  
  /*
      STEP 1 : Create a buffer to store recieved data.
  */  

  char recvBuff[1024];

  /*
      STEP 2 : Clear buffer and fill it with zeros.
  */  

  memset(recvBuff, '0', sizeof(recvBuff));
  struct sockaddr_in serv_addr;

  /* Create a socket first */

  /*
      STEP 3 : Open a socket
  */  

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\nError : Could not create socket \n");
    return 1;
  }

  /* Initialize sockaddr_in data structure */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000); // port
  char ip[50];

  /* Ask IP address from user if not entered in command line arguments */
  if (argc < 2) {
    printf("Enter IP address to connect: ");
    scanf("%s", ip);
  } 
  else {
    strcpy(ip, argv[1]);
  }

  serv_addr.sin_addr.s_addr = inet_addr(ip);

  /*
      STEP 4 : Attempt connection
  */  

  /* Attempt a connection */
  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

  printf("Connected to ip: %s : %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

  /* Create file where data will be stored */
  FILE *fp;
  char fname[100];
  read(sockfd, fname, 256);
  printf("File Name: %s\n", fname);
  printf("Receiving file...");
  fp = fopen(fname, "ab");
  if (NULL == fp) {
    printf("Error opening file");
    return 1;
  }
  long double sz = 1;
  /*
      STEP 5 : Start receiving data using read() method, 1024 bytes at a time. 
               And write buffer to file.
  */  

  /* Receive data in chunks of 1024 bytes */
  while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
    sz++;
    //printf("Received: %Le Mb", (sz / 1024));
    fflush(stdout);
    fwrite(recvBuff, 1, bytesReceived, fp);
  }

  if (bytesReceived < 0) {
    printf("\n Read Error \n");
  }

  printf("\nFile OK....Completed\n");
  return 0;
}
