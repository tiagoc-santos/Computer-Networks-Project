#include "protocol.h"
#include "utils.h"
#include "start.h"
#include "try.h"
#include "quit.h"

int player_udp_socket;
char* server_IP = SERVER_IP_DEFAULT;
char* server_port = SERVER_PORT_DEFAULT;
struct addrinfo *server_info;

int main(int argc, char** argv) {
    int game_running = 0;
    char player_id[PLID_SIZE];

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

    if (argc == 5){
        if (!strcmp(argv[1], "-n") && !strcmp(argv[3], "-p")){
            server_IP = argv[2];
            server_port = argv[4];
        }
        else{
            printf("Invalid arguments\n");
            exit(1);
        }
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
        memset(cmd, 0, CMD_SIZE);
        read_line(cmd);
        memset(cmd_args, 0, ARG_SIZE*CMD_SIZE);

        // If the command is empty
        if(strlen(cmd) == 0){
            continue;
        }
        split_line(cmd, cmd_args);

        // start game command
        if (!strcmp(cmd_args[0], "start")){
            if(game_running){
                fprintf(stderr, "Game already running.\nYou need to quit first.\n");
                continue;
            }

            if (validate_start(cmd_args) != 0)
                fprintf(stderr, "Invalid command.\n");

            //funcao start
            else{
                strcpy(player_id, cmd_args[1]);
                player_id[6] = '\0';
                start_game(cmd_args[1], cmd_args[2]);
                game_running = 1;
            }
        }

        // debug command
        else if (!strcmp(cmd_args[0], "debug")){
            
        }
        
        // try command
        else if (!strcmp(cmd_args[0], "try") && game_running){
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
            if(quit_game(player_id) != 0)
                fprintf(stderr, "Unable to quit\n");
            
            else
                game_running = 0;

        }

        // exit command
        else if (!strcmp(cmd_args[0], "exit")){
            if(game_running){
                if(quit_game(player_id) != 0){
                    fprintf(stderr, "Unable to quit\n");
                }
                game_running = 0;
            }
            break;
        }
    }

    free(server_info);
    close(player_udp_socket);

    return 0;
}