#include "headers/protocol.h"
#include "headers/start.h"
#include "headers/try.h"
#include "headers/debug.h"
#include "headers/quit.h"
#include "headers/show_trials.h"
#include "headers/scoreboard.h"

int handle_client_udp(){
    char message[MSG_SIZE], message_args[ARG_SIZE][CMD_SIZE];

    if(read_message_udp(message) == -1)
        return -1;

    split_line(message, message_args);
    char* op_code = message_args[0];
    
    // start command
    if (!strcmp(op_code, "SNG")){
        reply_start(message_args);
    }
    
    // debug command
    else if (!strcmp(op_code, "DBG")){
        reply_debug(message_args);
    }
    
    // try command
    else if (!strcmp(op_code, "TRY")){
        reply_try(message_args);
    }

    // quit command
    else if (!strcmp(op_code, "QUT")){
        reply_quit(message_args);
    }

    else{
        char response[MSG_SIZE];
        strcpy(response, "ERR\n");
        send_message_udp(response);    
    }
    return 0;
}

int handle_client_tcp(){
    pid_t pid = fork();

    if(pid == -1){
        fprintf(stderr, "handle_client_tcp: Error forking:%s\n", strerror(errno));
        return -1;
    }

    // Parent process
    if(pid != 0)
        return 0;

    int socket = connect_client();
    if(socket == -1){
        fprintf(stderr, "handle_tcp_client: Error connectiong to client\n");
        return -1;
    }

    char message[MSG_SIZE], message_args[ARG_SIZE][CMD_SIZE];
    
    if(read_message_tcp(socket, message) == -1){
        fprintf(stderr, "handle_tcp_client: Error reading from client.\n");
        return -1;
    }

    split_line(message, message_args);
    char* op_code = message_args[0];

    // show trials command
    if (!strcmp(op_code, "STR")){
        reply_showtrials(socket, message_args);
    }
    
    // scoreboard command
    else if (!strcmp(op_code, "SSB")){
        reply_scoreboard(socket, message_args);
    }

    else{
        char response[MSG_SIZE];
        strcpy(response, "ERR1\n");
        send_tcp_message(socket, response, 0, NULL, NULL);
    }
    
    close(socket);
    return 0;
}

int main(int argc, char** argv){

    // Ignore SIGPIPE and SIGCHILD
    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    if((sigaction(SIGPIPE, &act, NULL) == -1) || 
        (sigaction(SIGCHLD, &act, NULL) == -1))
        exit(1);

    if(argc == 2){
        if (!strcmp(argv[1], "-v")){
            verbose = 1;
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
    }

    else if(argc == 3){
        if (!strcmp(argv[1], "-p")){
            port = argv[2];
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
    }

    else if (argc == 4){
        if (!strcmp(argv[1], "-p") && !strcmp(argv[3], "-v")){
            port = argv[2];
            verbose = 1;
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
    }

    if(init_udp_socket() == -1)
        return -1;
    
    if(init_tcp_socket() == -1)
        return -1;
    
    // File descriptor for select
    fd_set read_file_descriptors;
    
    while (1){
        FD_ZERO(&read_file_descriptors);

        //Add UDP socket to set
        FD_SET(udp_socket, &read_file_descriptors);

        //Add TCP socket to set
        FD_SET(tcp_socket, &read_file_descriptors);
        
        int max_fd = (udp_socket > tcp_socket) ? udp_socket : tcp_socket;
        int counter = select(max_fd + 1, &read_file_descriptors, (fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);

        if(counter < 0){
            fprintf(stderr, "select error\n");
            break;
        }

        //If UDP socket is ready
        if(FD_ISSET(udp_socket, &read_file_descriptors))
            handle_client_udp();

        if(FD_ISSET(tcp_socket, &read_file_descriptors))
            handle_client_tcp();
    }

    if(close_udp_socket() != 0)
        fprintf(stderr, "Error closing udp socket.\n");
}