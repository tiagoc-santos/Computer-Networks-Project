#ifndef PROTOCOL_H
#define PROTOCOL_h

#define SERVER_IP_DEFAULT "localhost"
#define SERVER_PORT_DEFAULT "58019"

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>

int get_server_info(struct addrinfo** res, char* server_IP, char* server_port, int udp);
int create_socket(int udp);

#endif