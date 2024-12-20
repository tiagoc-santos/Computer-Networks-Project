#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_IP_DEFAULT "localhost"
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
#include <fcntl.h>
#include <sys/stat.h>
#include "../../common/utils.h"

extern char* port;

// UDP
extern int udp_socket, verbose;
extern socklen_t udp_addrlen;
extern struct addrinfo* udp_res;
extern struct sockaddr_in udp_addr;

int init_udp_socket();
int send_message_udp(char message[MSG_SIZE]);
int read_message_udp(char message[MSG_SIZE]);
int close_udp_socket();

// TCP
extern int tcp_socket;
extern socklen_t tcp_addrlen;
extern struct addrinfo* tcp_res;
extern struct sockaddr_in tcp_addr;

int init_tcp_socket();
int connect_client();
int read_message_tcp(int socket, char message[MSG_SIZE]);
int send_tcp_message(int socket, char response[MSG_SIZE], int fsize, 
                        char fname[FILENAME_SIZE], char fcontents[FILE_SIZE]);
int close_tcp_socket();

#endif