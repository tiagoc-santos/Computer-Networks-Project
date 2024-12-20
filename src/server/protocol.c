#include "headers/protocol.h"

char* port = SERVER_PORT_DEFAULT;
int udp_socket, tcp_socket,verbose = 0;
socklen_t udp_addrlen, tcp_addrlen;
struct addrinfo *udp_res, *tcp_res;
struct sockaddr_in udp_addr, tcp_addr;

/*---------------------------------UDP---------------------------------*/

int init_udp_socket(){
    struct addrinfo hints;
    int errcode;

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if(udp_socket == -1){
        fprintf(stderr, "Error creating udp socket\n");
        return -1;
    }
        
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

    if((errcode = getaddrinfo("localhost", port, &hints, &udp_res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(errcode));
        return -1;
    }

    if(bind(udp_socket, udp_res->ai_addr, udp_res->ai_addrlen) != 0){
        fprintf(stderr, "Error while binding: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int send_message_udp(char message[MSG_SIZE]){
    size_t bytes_sent;

    bytes_sent = sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&udp_addr, udp_addrlen);
    if (bytes_sent == -1){
        fprintf(stderr, "Error sending message.\n");
        return -1;
    }
    if(verbose)
        fprintf(stdout, "Sent UDP response (%ld bytes): %sTo IP: %s and port: %u\n",\
        bytes_sent, message, inet_ntoa(udp_addr.sin_addr), ntohs(udp_addr.sin_port));
    return 0;
}

int read_message_udp(char message[MSG_SIZE]){
    size_t bytes_received;
    
    udp_addrlen = sizeof(udp_addr);

    bytes_received = recvfrom(udp_socket, message, MSG_SIZE, 0, (struct sockaddr *)&udp_addr, &udp_addrlen);
    
    if (bytes_received == -1){
        fprintf(stderr, "Error reading from udp socket\n");
        return -1;
    }
    message[bytes_received] = '\0';
    
    if(verbose)
        fprintf(stdout, "Received UDP request (%ld bytes): %sFrom IP: %s and port: %u\n",\
        bytes_received, message, inet_ntoa(udp_addr.sin_addr), ntohs(udp_addr.sin_port));
    
    return 0;
}

int close_udp_socket(){
    freeaddrinfo(udp_res);
    if(close(udp_socket) != 0)
        return -1;
    return 0;
}

/*------------------------- TCP Requests -------------------------*/

int init_tcp_socket() {
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((errcode = getaddrinfo("localhost", port, &hints, &tcp_res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(errcode));
        return -1;
    }

    tcp_socket = socket(tcp_res->ai_family, tcp_res->ai_socktype, tcp_res->ai_protocol);
    if (tcp_socket == -1){
        fprintf(stderr, "Error creating tcp socket\n");
        return -1;
    }

    if(bind(tcp_socket, tcp_res->ai_addr,tcp_res->ai_addrlen) != 0){
        close(tcp_socket);
        fprintf(stderr, "Binding error: %s\n", strerror(errno));
        return -1;
    }
    
    if(listen(tcp_socket, 5) == -1){
        close(tcp_socket);
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int connect_client(){
    int socket = accept(tcp_socket, (struct sockaddr *) &tcp_addr, &tcp_addrlen);
    
    if(socket == -1)
        return -1;

    /*Set timeout for operations*/
    struct timeval timeout;
    timeout.tv_sec = 5; // 5 second tieout
    timeout.tv_usec = 0;

    if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) != 0){
        fprintf(stderr, "connect_client: Error setting timeout;: %s\n", strerror(errno));
        close(socket);
        return -1;
    }
    
    return socket;
}

int read_message_tcp(int socket, char message[MSG_SIZE]){
    int i = 0, nread = 0;
    char buffer[MSG_SIZE];

    memset(message, 0, MSG_SIZE);

    while(1) {
        nread = read(socket,buffer, 1);
        
        if(nread == -1){
            return -1;
        }
        
        else if(buffer[0] == '\n'){
            message[i++] = buffer[0];
            break;
        }

        message[i++] = buffer[0];
    }
    message[i] = '\0';

    if(verbose)
        fprintf(stdout, "Received TCP request (%ld bytes) %sFrom IP: %s and port: %u\n",\
        strlen(message), message, inet_ntoa(tcp_addr.sin_addr), ntohs(tcp_addr.sin_port));

    return 0;
}

int send_tcp_message(int socket, char response[MSG_SIZE], int fsize, 
                        char fname[FILENAME_SIZE], char fcontents[FILE_SIZE]){
    
    char* ptr, buffer[BUFFER_SIZE], message[BUFFER_SIZE];
    int nbytes, nwritten;
    int nleft;

    if(fsize != 0){
        sprintf(message, "%s %s %d %s\n", response, fname, fsize, fcontents);
    }
    else{
        sprintf(message, "%s", response);
    }

    nbytes = strlen(message);
    nleft = nbytes;
    ptr = strcpy(buffer, message);
    
    while(nleft > 0){
        nwritten = write(socket, ptr, nleft);
        if(nwritten == -1){
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    if(verbose){
        if(fsize == 0){
            fprintf(stdout, "Sent TCP response (%d bytes): %sTo IP: %s and port: %u\n",\
            nwritten, message, inet_ntoa(tcp_addr.sin_addr), ntohs(tcp_addr.sin_port));
        }
        else{
            fprintf(stdout, "Sent TCP response (%d bytes): \"%s\"\nTo IP: %s and port: %u\n",\
            nwritten, fname, inet_ntoa(tcp_addr.sin_addr), ntohs(tcp_addr.sin_port));
        }
    }

    return 0;
}

int close_tcp_socket(){
    freeaddrinfo(tcp_res);
    if(close(tcp_socket) != 0)
        return -1;
    return 0;
}