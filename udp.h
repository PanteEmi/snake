#ifndef UDPCONTROL
#define UDPCONTROL

#define PORT 8080
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


struct UDP_client{
    int sockfd;
    struct sockaddr_in client_addr;
};


struct UDP_client create_socket(const unsigned int port, const char* ip_addr);

ssize_t receive_from_socket(struct UDP_client client, char *buffer);


void close_socket(int sockfd);


int socket_wait_for_data(int fd, int timeout_ms);



#endif