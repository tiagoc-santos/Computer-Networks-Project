#include "./headers/start.h"

int validate_start(char cmd_args[ARG_SIZE][CMD_SIZE]) {
    return (num_args(cmd_args) != 3 || strlen(cmd_args[1]) != 6 || !check_digits(cmd_args[1])
                || !check_digits(cmd_args[2]) || atoi(cmd_args[2]) > 600 || atoi(cmd_args[2]) < 1);
}

int start_game(char PLID[PLID_SIZE], char time[ARG_SIZE]){
    char message[MSG_SIZE], response[MSG_SIZE];
    int time_num = atoi(time);
    
    sprintf(message, "SNG %s %03d\n", PLID, time_num);
    if(send_udp_request(message, strlen(message), player_udp_socket, server_info, response) == -1)
        return -1;

    if (!strcmp(response, "RSG OK\n"))
        fprintf(stdout, "New game started (max  %d seconds)\n", time_num);
    
    else if (!strcmp(response, "RSG NOK\n")){
        fprintf(stdout, "Game ongoing. Quit the game to start a new one.\n");
        return 1;
    }

    else if (!strcmp(response, "ERR\n")){
        fprintf(stderr, "Error starting game.\n");
        return -1;
    }
    return 0;
}

