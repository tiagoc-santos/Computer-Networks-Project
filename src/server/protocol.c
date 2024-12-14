#include "headers/protocol.h"

char* port = SERVER_PORT_DEFAULT;
int udp_socket, tcp_socket,verbose = 0;
socklen_t udp_addrlen;
struct addrinfo* udp_res;
struct sockaddr_in udp_addr;

/*---------------------------------UDP---------------------------------*/

int init_udp_socket(){
    struct addrinfo hints;
    int errcode;

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

    if((errcode = getaddrinfo("localhost", port, &hints, &udp_res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(errcode));
        return -1;
    }

    if(bind(udp_socket, udp_res->ai_addr, udp_res->ai_addrlen) != 0){
        fprintf(stderr, "Error while binding\n");
        return -1;
    }

    return 0;
}

//TO-DO
int init_tcp_socket(){
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
        fprintf(stdout, "Sent UDP response (%ld bytes): %s", bytes_sent, message);
    
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
        fprintf(stdout, "Received UDP request (%ld bytes): %s", bytes_received, message);
    
    return 0;
}

int close_udp_socket(){
    freeaddrinfo(udp_res);
    if(close(udp_socket) != 0)
        return -1;
    return 0;
}
