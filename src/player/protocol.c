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

/*------------------------- UDP Requests -------------------------*/

int send_message_udp(int socket_fd, struct addrinfo* res, char* message, int message_size){
    size_t bytes_sent;

    bytes_sent = sendto(socket_fd, message, message_size, 0, res->ai_addr, res->ai_addrlen);
    if (bytes_sent == -1){
        fprintf(stderr, "Error sending message.\n");
        return -1;
    }
    return 0;
}

int receive_message_udp(int socket_fd, struct addrinfo* res, char* buffer){
    size_t bytes_received;
    struct sockaddr_in addr;
    socklen_t addrlen;
    
    addrlen = sizeof(addr);

    bytes_received = recvfrom(socket_fd, buffer, 8192, 0, (struct  dsockaddr*)&addr, &addrlen);
  
    if (bytes_received == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            fprintf(stderr, "Timeout receiving message.\n");
            return TIMEOUT;
        }
        else{
            fprintf(stderr, "Error receiving message.\n");
            return -1;
        }
    }
    return 0;
}

int send_udp_request(char* message, int message_size, int socket_fd, struct addrinfo* res,
                        char* buffer){
    int n = 0, ret;
    
    while(n < MAX_RESENDS){
        if (send_udp_message(socket_fd, res, message, message_size) == -1) {
            return -1;
        }
        //TODO: maybe não escreve no buffer (potential bug)
        ret = receive_message_udp(socket_fd, res, buffer);
        if (ret != 0){
            if (ret == TIMEOUT){
                continue;
            }
            // Other unknown error occured
            else{
                return -1;
            }
        }
        else{
            break;
        }
    }
    return 0;
}