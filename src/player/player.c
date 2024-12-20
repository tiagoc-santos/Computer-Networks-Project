#include "./headers/protocol.h"
#include "../common/utils.h"
#include "./headers/start.h"
#include "./headers/try.h"
#include "./headers/quit.h"
#include "./headers/debug.h"
#include "./headers/show_trials.h"
#include "./headers/scoreboard.h"

int player_udp_socket, game_running = 0;
char* server_IP = SERVER_IP_DEFAULT;
char* server_port = SERVER_PORT_DEFAULT;
struct addrinfo *server_info;
char player_id[PLID_SIZE];

// Handles SIGINT
void handle_sigint(int signal){
    if(game_running){
        if(quit_game(player_id) != 0)
            fprintf(stderr, "Unable to exit\n");
    }
    free(server_info);
    if(close(player_udp_socket) != 0){
        fprintf(stderr, "Error closing udp socket...\n");
        exit(1);
    }
    exit(0);
}

int main(int argc, char** argv) {
    int nT = 1;
    
    // Ignore SIGPIPE and define handler for SIGINT
    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    if(sigaction(SIGPIPE, &act, NULL) == -1 || signal(SIGINT, handle_sigint) == SIG_ERR) 
        exit(1);

    player_id[0] = '\0';

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

    if (get_server_info(&server_info, server_IP, server_port, 1) != 0)
        exit(1);
    

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

            if(strcmp(player_id, cmd_args[1]) && game_running){
                fprintf(stderr, "There's another player already playing\n");
                continue;
            }

            strcpy(player_id, cmd_args[1]);
            player_id[6] = '\0';
            int status = start_game(player_id, cmd_args[2]);
            
            //ERR has occured
            if(status == -1)
                break;
            else if( status != 0){
                continue;
            }
            game_running = 1;
            nT = 1; 
        }

        // debug command
        else if (!strcmp(cmd_args[0], "debug")){
            if(validate_debug(cmd_args) != 0){
                fprintf(stderr, "Invalid command.\n");
                continue;
            }

            if(strcmp(player_id, cmd_args[1]) && game_running){
                fprintf(stderr, "There's another player already playing\n");
                continue;
            }
            
            char colors[SIZE_COLORS];
            sprintf(colors, "%c %c %c %c", cmd_args[3][0], cmd_args[4][0], cmd_args[5][0], cmd_args[6][0]);
            strcpy(player_id, cmd_args[1]);
            player_id[6] = '\0';
            int ret_debug = debug_game(player_id, atoi(cmd_args[2]), colors);
            
            //ERR has occurred
            if(ret_debug == -1)
                break;
            else if(ret_debug != 0){
                continue;
            }
            game_running = 1;
            nT = 1;
        }
        
        // try command
        else if (!strcmp(cmd_args[0], "try") && game_running){
            if (validate_try(cmd_args) != 0){
                fprintf(stderr, "Invalid command.\n");
                continue;
            }
            int ret_try = try(player_id, cmd_args[1], cmd_args[2], cmd_args[3], cmd_args[4], &nT);
            
            // ERR has occurred
            if(ret_try == -1)
                break;
            else if(ret_try == GAME_ENDED)
                game_running = 0;
        }

        // show trials command
        else if ((!strcmp(cmd_args[0], "st") || !strcmp(cmd_args[0], "show_trials"))){
            if(player_id[0] == '\0'){
                fprintf(stdout, "No PLID associated. Start a game first.\n");
                continue;
            }
            
            int ret_st = show_trials(player_id);
            
            //ERR has occurred
            if(ret_st == -1)
                break;
            else
                continue;
        }

        // scoreboard command
        else if (!strcmp(cmd_args[0], "sb") || !strcmp(cmd_args[0], "scoreboard")){
            int ret_sb = scoreboard();
            
            //ERR has occurred
            if (ret_sb == -1)
                break;
            else if(ret_sb != 0){
                fprintf(stdout, "No games in record...\n");
                continue;
            }
        }

        // quit command
        else if (!strcmp(cmd_args[0], "quit")){
            if(player_id[0] == '\0'){
                fprintf(stdout, "There is no game ongoing...\n");
                continue;
            }
            int ret_quit = quit_game(player_id);
            
            //ERR has occurred
            if(ret_quit == -1)
                break;
            else if(ret_quit != 0){
                fprintf(stderr, "Unable to quit\n");
                continue;
            }
            game_running = 0;
        }

        // exit command
        else if (!strcmp(cmd_args[0], "exit")){
            //If a game is running informs the server
            if(game_running){
                int ret_exit = quit_game(player_id);

                if(ret_exit == -1)
                    break;
                else if(ret_exit != 0){
                    fprintf(stderr, "Unable to exit\n");
                    continue;
                }
                game_running = 0;
            }
            break;
        }
        else{
            fprintf(stderr, "Invalid command.\n");
            continue;
        }
    }

    free(server_info);
    if(close(player_udp_socket) != 0){
        fprintf(stderr, "Error closing udp socket...\n");
        exit(1);
    }

    return 0;
}