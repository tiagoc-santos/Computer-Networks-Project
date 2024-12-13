#include "./headers/debug.h"

int validate_colors(char cmd_args[ARG_SIZE][CMD_SIZE]){
    int i;
    for (i = 3; i <= 6; i++){
        if (!is_valid_color(cmd_args[i]))
            return 1;
    }
    return 0;
}


int validate_debug(char cmd_args[ARG_SIZE][CMD_SIZE]){
    return (num_args(cmd_args) != 7 || strlen(cmd_args[1]) != 6 || !check_digits(cmd_args[1])
                || !check_digits(cmd_args[2]) || atoi(cmd_args[2]) > 600 || atoi(cmd_args[2]) < 1) || validate_colors(cmd_args);
}

int debug_game(char PLID[PLID_SIZE], int max_playtime, char colors[SIZE_COLORS]){
    
    char message[MSG_SIZE], response[MSG_SIZE];
    
    sprintf(message, "DBG %s %03d %s\n", PLID, max_playtime, colors);
    if(send_udp_request(message, strlen(message), player_udp_socket, server_info, response) == -1)
        return -1;
    
    if(!strcmp(response, "RDB OK\n")){
        fprintf(stdout,"Debug session started with key: %s\n", colors);
    }

    else if(!strcmp(response, "RDB NOK\n")){
        fprintf(stdout, "Game ongoing. Quit the game to start a new debug session.\n");
        return 1;
    }

    else if(!strcmp(response, "ERR\n")){
        fprintf(stderr, "Error occured when starting debug session.\n");
        return -1;
    }  
    return 0;
}