#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>

#define  BUFF_SIZE 1024

void client_listener(int client_socket) {
  if (client_socket == -1) {
    printf("client accept fail");
    return;
  }
  time_t start_time = time(NULL);
  time_t current_time = time(NULL);
  double time_passed = 0;

  char buff[BUFF_SIZE];
  while (time_passed < 10) {
    read (client_socket, buff, BUFF_SIZE);
    printf("[%d][%f] receive: %s\n", client_socket, time_passed, buff);
    write(client_socket, buff, strlen(buff)+1);
    current_time = time(NULL);
    time_passed = difftime(current_time, start_time); 
  }
  printf("[%d] connection closed\n", client_socket); 
  close(client_socket);
}

int main(int argc, char** argv) {
  int server_socket;
  int client_socket;
  int client_addr_size;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;


  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if(server_socket == -1){
    printf("server socket creation failed\n");
    exit(1);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    printf("bind failed\n");
    exit(1);
  }
  if(listen(server_socket, 5) == -1){
    printf("listen failed\n");
    exit(1);
  }
  while(1) {
    client_addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_addr_size);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, client_listener, client_socket);
    pthread_detach(thread_id);
  }
  close(server_socket);
  
  return 1;
}

