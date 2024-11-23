#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_IP_DEFAULT "localhost"
#define SERVER_PORT_DEFAULT "58019"
#define MAX_RESENDS 5
#define TIMEOUT 1

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

int get_server_info(struct addrinfo** res, char* server_IP, char* server_port, int udp);
int create_socket(int udp);
int send_message_udp(int socket_fd, struct addrinfo* res, char* message, int message_size);
int receive_message_udp(int socket_fd, struct addrinfo* res, char* buffer);
int send_udp_request(char* message, int message_size, int socket_fd, struct addrinfo* res, char* buffer);

#endif