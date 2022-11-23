#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>

#define BUFF_SIZE 1024
#define CONNECT_TIME 30

int echo = 0;
int broadcast = 0;

int* handle_client(int mode, int client_socket) {
  int static clients[1024] = {};
  int static client_index = 0;

  if (mode == 0) {
    return clients;
  } else if (mode == 1) {
    clients[client_index] = client_socket;
    printf("added client %d at index %d\n", client_socket, client_index);
    client_index += 1;
  } else if (mode == 2) {
    for(int i = 0; i < client_index; i++) {
      if (clients[i] == client_socket) {
        clients[i] = -1;
        printf("removed client %d at index %d\n", client_socket, i);
      }
    }
  } else {
    printf("invalid mode %d, available mode is 0, 1, 2\n");
    exit(1);
  }
}


void client_listener(int client_socket) {
  if (client_socket == -1) {
    printf("client accept fail");
    return;
  }
  time_t start_time = time(NULL);
  time_t current_time = time(NULL);
  double time_passed = 0;

  char buff[BUFF_SIZE];
  while(time_passed < CONNECT_TIME) {
    read(client_socket, buff, BUFF_SIZE);
    printf("[%d][%f] receive: %s\n", client_socket, time_passed, buff);
    if (echo || broadcast) {
      write(client_socket, buff, strlen(buff)+1);
    }
    if (broadcast) {
      int* clients = handle_client(0, NULL);
      while (*clients != 0) {
        if (*clients != -1 && *clients != client_socket) {
          write(*clients, buff, strlen(buff)+1);
        }
        clients += 1;
      }
    }
    current_time = time(NULL);
    time_passed = difftime(current_time, start_time); 
  }
  printf("[%d] connection closed\n", client_socket); 
  handle_client(2, client_socket);
  close(client_socket);
}

int main(int argc, char** argv) {
  int server_socket;
  int client_socket;
  int client_addr_size;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  if (argc != 3 && argc != 4) {
    printf("echo-server:\nsyntax : echo-server <port> [-e[-b]]\nsample : echo-server 1234 -e -b\n");
    exit(1);
  }

  for(int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0) {
      echo = 1;
    } else if (strcmp(argv[i], "-b") == 0) {
      broadcast = 1;
    }
  }
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
    handle_client(1, client_socket);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, client_listener, client_socket);
    pthread_detach(thread_id);
  }
  close(server_socket);
  
  return 1;
}

