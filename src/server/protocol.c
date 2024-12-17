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
        fprintf(stdout, "SENT UDP response (%ld bytes): %sTo IP: %s and port: %u\n",\
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

    if ((errcode = getaddrinfo(NULL, port, &hints, &tcp_res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(errcode));
        return -1;
    }

    tcp_socket = socket(tcp_res->ai_family, tcp_res->ai_socktype, tcp_res->ai_protocol);
    if (tcp_socket == -1){
        fprintf(stderr, "Error creating tcp socket\n");
        return -1;
    }

    if(bind(tcp_socket, tcp_res->ai_addr,tcp_res->ai_addrlen) != 0){
        fprintf(stderr, "Binding error: %s\n", strerror(errno));
        return -1;
    }
    
    if(listen(tcp_socket, 5) == -1){
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

