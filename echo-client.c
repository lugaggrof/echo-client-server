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

void server_listener(int server_socket) {
  if (server_socket == -1) {
    printf("server accept fail\n");
    return;
  }
  time_t start_time = time(NULL);
  time_t current_time = time(NULL);
  double time_passed = 0;

  char buff[BUFF_SIZE];
  while(time_passed < 10) {
    read(server_socket, buff, BUFF_SIZE);
    printf("[%d][%f] receive: %s\n", server_socket, time_passed, buff);
    current_time = time(NULL);
    time_passed = difftime(current_time, start_time);
  }
  printf("[%d] connection closed\n", server_socket);
  close(server_socket);
}


int main(int argc, char **argv) {

    if (argc != 3) {
      printf("echo-client:\nsyntax : echo-client <ip> <port>\nsample : echo-client 192.168.10.2 1234");
      exit(1);
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char send_buff[BUFF_SIZE];

    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
      printf("socket creation failed\n");
      exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(-1 == connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))){
      printf("connection to %s:%s failed\n", argv[1], argv[2]);
      exit(1);
    } 
    clock_t start_time = clock();
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, server_listener, client_socket);
    pthread_detach(thread_id);

    while (1) {
      scanf("%s", send_buff);
      printf("send: %s\n", send_buff);
      write(client_socket, send_buff, strlen(send_buff)+1);
      
      memset(send_buff, 0, sizeof(send_buff));
    }

    close(client_socket);
    return 0;
}

