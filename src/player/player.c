#include "protocol.h"
#include "utils.h"
#include "start.h"
#include "try.h"
#include "quit.h"
#include "debug.h"

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
            if (validate_start(cmd_args) != 0){
                fprintf(stderr, "Invalid command.\n");
                continue;
            }

            strcpy(player_id, cmd_args[1]);
            player_id[6] = '\0';
            if(start_game(player_id, cmd_args[2]) != 0){
                continue;
            }
            game_running = 1;
            
        }

        // debug command
        else if (!strcmp(cmd_args[0], "debug")){
            if(validate_debug(cmd_args) != 0){
                fprintf(stderr, "Invalid command.\n");
                continue;
            }
            char colors[SIZE_COLORS];
            sprintf(colors, "%c %c %c %c", cmd_args[3][0], cmd_args[4][0], cmd_args[5][0], cmd_args[6][0]);
            strcpy(player_id, cmd_args[1]);
            player_id[6] = '\0';
            if(debug_game(player_id, atoi(cmd_args[2]), colors) != 0){
                continue;
            }
            game_running = 1;
        }
        
        // try command
        else if (!strcmp(cmd_args[0], "try") && game_running){
            if (validate_try(cmd_args)){
                fprintf(stderr, "Invalid command.\n");
                continue;
            }
            //funcao try
        }

        // show trials command
        else if (!strcmp(cmd_args[0], "st") || !strcmp(cmd_args[0], "show_trials")){

        }

        // scoreboard command
        else if (!strcmp(cmd_args[0], "sb") || !strcmp(cmd_args[0], "scoreboard")){
            
        }

        // quit command
        else if (!strcmp(cmd_args[0], "quit")){
            if(quit_game(player_id) != 0){
                fprintf(stderr, "Unable to quit\n");
                continue;
            }
            game_running = 0;
        }

        // exit command
        else if (!strcmp(cmd_args[0], "exit")){
            if(game_running){
                if(quit_game(player_id) != 0){
                    fprintf(stderr, "Unable to exit\n");
                    continue;
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