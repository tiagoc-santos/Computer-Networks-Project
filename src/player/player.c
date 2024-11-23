#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "protocol.h"
#include <ctype.h>

#define CMD_SIZE 50
#define ARG_SIZE 10

int read_line(char *line){
    char c;
    int i = 0;

    memset(line, 0, CMD_SIZE);
    while((c = getchar()) != '\n')
        line[i++] = c;
    line[i] = '\0';
    return 0;
}

void split_line(char *line, char arguments[ARG_SIZE][CMD_SIZE]) {
    char *token;
    int i = 0;

    token = strtok(line, " \t\n");
    while (token != NULL && i < ARG_SIZE) {
        strcpy(arguments[i++], token);
        token = strtok(NULL, " \t\n");
    }

    for (; i < ARG_SIZE; i++) {
        arguments[i][0] = '\0';
    }
}

int num_args(char arguments[ARG_SIZE][CMD_SIZE]){
    int res = 0;
    int i = 0;
    while (i < ARG_SIZE){
        if (arguments[i++][0] == '\0')
            break;
        res++;
    }
    return res;
}

int check_digits(char* num_str){
    int length = strlen(num_str);
    int i;
    for (i = 0; i < length; i++){
        if (!isdigit(num_str[i]))
            return 0;
    }
    return 1;
}

int validate_start(char cmd_args[ARG_SIZE][CMD_SIZE]){
    return (num_args(cmd_args) != 3 || strlen(cmd_args[1]) != 6 || !check_digits(cmd_args[1])
                || !check_digits(cmd_args[2]) || atoi(cmd_args[2]) > 600 || atoi(cmd_args[2]) < 1);
}

int is_valid_color(char* color){
    return (!strcmp(color, "R") || !strcmp(color, "G") || !strcmp(color, "B") || !strcmp(color, "Y") ||
                !strcmp(color, "O") || !strcmp(color, "P"));
}

int validate_try(char cmd_args[ARG_SIZE][CMD_SIZE]){
    int i;

    for (i = 1; i < 5; i++){
        if (!is_valid_color(cmd_args[i]))
            return 0;
    }
    return 1;
}

int main(int argc, char** argv){
    char* server_IP = SERVER_IP_DEFAULT;
    char* server_port = SERVER_PORT_DEFAULT;
    struct addrinfo *server_info;
    int player_udp_socket;
    int game_running = 0;

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

    if (get_server_info(&server_info, server_IP, server_port, 1) != 0){
        fprintf(stderr, "Error getting server info.\n");
        exit(1);
    }

    player_udp_socket = create_socket(1);
    if (player_udp_socket == -1){
        fprintf(stderr, "Error creating udp socket.\n");
        exit(1);
    }

    while(1){
        char cmd[CMD_SIZE], cmd_args[ARG_SIZE][CMD_SIZE];
        read_line(cmd);

        // If the command is empty
        if(strlen(cmd) == 0)
            continue;
        
        split_line(cmd, cmd_args);

        // start game command
        if (!strcmp(cmd_args[0], "start")){
            if (!validate_start(cmd_args))
                fprintf(stderr, "Invalid command.\n");

            //funcao start
            else{
            }
        }

        // start game command
        else if (!strcmp(cmd_args[0], "start")){
            if (!validate_start(cmd_args))
                fprintf(stderr, "Invalid command.\n");

            //funcao start
            else{
                game_running = 1;
            }
        }

        // try command
        else if (!strcmp(cmd_args[0], "try")){
            if (!validate_try(cmd_args))
                fprintf(stderr, "Invalid command.\n");

            //funcao try
            else{
            }
        }

        // show trials command
        else if (!strcmp(cmd_args[0], "st") || !strcmp(cmd_args[0], "show_trials")){

        }

        // scoreboard command
        else if (!strcmp(cmd_args[0], "sb") || !strcmp(cmd_args[0], "scoreboard")){
            
        }

        // quit command
        else if (!strcmp(cmd_args[0], "quit")){
            if (game_running)
                //manda msg  e meter game running a 0  
                continue; 
        }

        // exit command
        else if (!strcmp(cmd_args[0], "exit")){
            
        }

        // debug command
        else if (!strcmp(cmd_args[0], "debug")){
            
        }
    }

    return 0;
}