#include <stdio.h> 
#include <string.h>
#include <unistd.h> // write(), close(), sleep()
#include <arpa/inet.h> // listen(), accept(), bind(), htons()

struct sockaddr_in c_addr;
char fname[100];

void SendFileToClient(int *arg) {
  int connfd = (int) *arg;
  printf("Connection accepted and id: %d\n", connfd);
  printf("Connected to Clent: %s:%d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
  write(connfd, fname, 256);

  FILE *fp = fopen(fname, "rb");
  if (fp == NULL) {
    printf("File opern error");
  }

  /*Read data from file and send it */
  while (1) {
    /*First read file in chunks of 1024 bytes */
    unsigned char buff[1024] = {
      0
    };
    int nread = fread(buff, 1, 1024, fp);
    printf("Bytes read %d \n", nread);        

    /*If read was success, send data. */
    if (nread > 0) {
      printf("Sending \n");
      write(connfd, buff, nread);
    }
    if (nread < 1024) {
      if (feof(fp)) {
        printf("End of file\n");
        printf("File transfer completed for id: %d\n", connfd);
      }
      if (ferror(fp))
        printf("Error reading\n");
      break;
    }
  }
  printf("Closing Connection for id: %d\n", connfd);
  close(connfd);
  shutdown(connfd, SHUT_WR);
  sleep(2);
}

int main(int argc, char *argv[]) {
  int connfd = 0, err;
  pthread_t tid;
  struct sockaddr_in serv_addr;
  int listenfd = 0, ret;
  char sendBuff[1024];
  int numrv;
  size_t clen = 0, slen = 0;
  
  /*
      STEP 1 : Open a socket to listen
  */  

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    printf("Error in socket creation\n");
    return -1;
  }

  printf("Socket retrieve success\n");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(5000);
  slen = sizeof(serv_addr);

  /*
      STEP 2 : Binding for socket 
  */  

  ret = bind(listenfd, (struct sockaddr *) &serv_addr, slen);
  if (ret < 0) {
    printf("Error in bind\n");
    return -1;
  }
  /*
      STEP 3 : Start listening for client requests
  */  

  if (listen(listenfd, 10) == -1) {
    printf("Failed to listen\n");
    return -1;
  }

  if (argc < 2) {
    printf("Enter file name to send: ");
    scanf("%s", fname);
  } else
    strcpy(fname, argv[1]);

  /*
      STEP 4 : Accept any new connections, create new thread for each connection. Do until user stops it.
  */  
  
  while (1) {
    clen = sizeof(c_addr);
    printf("Waiting...\n");
    connfd = accept(listenfd, (struct sockaddr *) &c_addr, (socklen_t *) &clen);
    if (connfd < 0) {
      printf("Error in accept\n");
      continue;
    }
    err = pthread_create( &tid, NULL, &SendFileToClient, &connfd);
    if (err != 0)
      printf("\ncan't create thread :[%s]", strerror(err));
  }
  /*
      STEP 5 : Close and release socket.
  */  

  close(connfd);
  return 0;
}
