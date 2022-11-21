#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {

    if (argc != 3) {
      printf("echo-client:\nsyntax : echo-client <ip> <port>\nsample : echo-client 192.168.10.2 1234");
      exit(1);
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char send_buff[BUFF_SIZE];
    char recv_buff[BUFF_SIZE+5];

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

    while (1) {
      scanf("%s", send_buff);
      printf("send: %s\n", send_buff);
      write(client_socket, send_buff, strlen(send_buff)+1);
      read (client_socket, recv_buff, BUFF_SIZE);
      
      printf("echo: %s\n", recv_buff);
      
      memset(send_buff, 0, sizeof(send_buff));
      memset(recv_buff, 0, sizeof(recv_buff));
    }

    close(client_socket);
    return 0;
}

