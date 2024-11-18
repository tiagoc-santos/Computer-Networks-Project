#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_IP_DEFAULT "localhost"
#define SERVER_PORT_DEFAULT "58019"

int get_server_info(struct addrinfo** res, char* server_IP, char* server_port, int udp){
    int errcode;
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4
    udp ? hints.ai_socktype=SOCK_DGRAM : hints.ai_socktype=SOCK_STREAM;

    if((errcode = getaddrinfo(server_IP, server_port, &hints, &res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n",gai_strerror(errcode));
        return -1;
    }
    return 0;
}

int main(int argc, char** argv){
    char* server_IP = SERVER_IP_DEFAULT;
    char* server_port = SERVER_PORT_DEFAULT;
    struct addrinfo *server_info;


    if(argc == 3){
        if (!strcmp(argv[1], "-n")){
            server_IP = argv[2];
        }
        else if (!strcmp(argv[1], "-p")){
            server_port = argv[2];
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    if (argc == 5)
        if (!strcmp(argv[1], "-n") && !strcmp(argv[3], "-p")){
            server_IP = argv[2];
            server_port = argv[4];
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
    if (get_server_info(&server_info, server_IP, server_port, 1))
        exit(1);

    while(1){
        
    }
}