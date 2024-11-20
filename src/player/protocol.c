#include "protocol.h"

int get_server_info(struct addrinfo** res, char* server_IP, char* server_port, int udp){
    int errcode;
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;

    if ((errcode = getaddrinfo(server_IP, server_port, &hints, res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n",gai_strerror(errcode));
        return -1;
    }
    return 0;
}

int create_socket(int udp){
    int socket_fd;
    struct timeval timeout;

    if (udp){
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket
    }
    else{
        socket_fd = socket(AF_INET, SOCK_STREAM, 0); //TCP socket 
    }

    if (socket_fd != 0){
        fprintf(stderr, "Error creating socket.\n");
        if (close(socket_fd) != 0){
            fprintf(stderr, "Error closing the socket.\n");
            return -1;
        }
        return -1;
    }

    /*Set timeout for operations*/
    timeout.tv_sec = 5;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        fprintf(stderr, "Error setting timeout.\n");
        if (close(socket_fd) != 0){
            fprintf(stderr, "Error closing the socket.\n");
            return -1;
        }
        return -1;
    }
    return socket_fd;
}