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

    if (socket_fd == -1){
        close(socket_fd);
        return -1;
    }

    /*Set timeout for operations*/
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        fprintf(stderr, "Error setting timeout.\n %s\n", strerror(errno));
        close(socket_fd);
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

    bytes_received = recvfrom(socket_fd, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &addrlen);
    
    if (bytes_received == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            return TIMEOUT;
        }
        else{
            return -1;
        }
    }
    buffer[bytes_received] = '\0';
    return 0;
}

int send_udp_request(char* message, int message_size, int socket_fd, struct addrinfo* res, char* buffer){
    int n = 0, ret;
    
    while(n < MAX_RESENDS){
        if (send_message_udp(socket_fd, res, message, message_size) == -1) {
            fprintf(stderr, "Error starting game.\n");
            return -1;
        }
        ret = receive_message_udp(socket_fd, res, buffer);
        if (ret != 0){
            if (ret == TIMEOUT){
                fprintf(stderr, "TIMEOUT trying again...\n");
                n++;
                continue;
            }
            // Other unknown error occured
            else{
                fprintf(stderr, "Error starting game.\n");
                return -1;
            }
        }
        else{
            return 0;
        }
    }
    fprintf(stderr, "Could not reach the server. Try again later.\n");
    return -1;
}

/*------------------------- TCP Requests -------------------------*/

int connect_server(int tcp_socket, struct addrinfo* res){
    if(connect(tcp_socket, res->ai_addr, res->ai_addrlen) == -1){
        fprintf(stderr, "Error occured while connecting to server.\n");
        if(close(tcp_socket) == -1){
            fprintf(stderr, "Error closing socket.\n");
        }
        return -1;
    }

    return 0;
}

int write_message_tcp(int tcp_socket, char* message){
    char* ptr, buffer[MSG_SIZE];
    int nbytes = strlen(message), nwritten;
    int nleft = nbytes;
    ptr = strcpy(buffer, message);
    while(nleft>0){
        nwritten = write(tcp_socket,ptr,nleft);
        if(nwritten == -1){
            fprintf(stderr, "Error sending the message.\n");
            if(close(tcp_socket) == -1){
                fprintf(stderr, "Error closing socket.\n");
            }
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    return 0;
}

int read_message_tcp(int tcp_socket, char buffer[BUFFER_SIZE]){
    char* ptr;
    int nbytes = MSG_SIZE, nread;
    int nleft = nbytes;
    ptr = buffer;

    while(nleft > 0){
        nread = read(tcp_socket,ptr,nleft);
        if(nread == -1){
            fprintf(stderr, "Error reading message.\n");
            return -1;
        }
        else if(nread==0)
            break;

        nleft -= nread;
        ptr += nread;
    }
    return 0;
}