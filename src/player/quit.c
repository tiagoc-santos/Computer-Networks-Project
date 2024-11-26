#include "quit.h"

int quit_game(char PLID[ARG_SIZE]){
    char message[MSG_SIZE], response[MSG_SIZE], response_items[ARG_SIZE][CMD_SIZE];
    char answer[ANSWER_SIZE];

    sprintf(message, "QUT %s\n", PLID);
    
    if(send_udp_request(message, strlen(message), player_udp_socket, server_info, response) == -1)
        return -1;

    split_line(response, response_items);

    if(!strcmp(response_items[1], "OK")){
        sprintf(answer, "%s %s %s %s",response_items[2], response_items[3], response_items[4], response_items[5]);
        fprintf(stdout, "The answer was: %s\n", answer);
    }

    else if(!strcmp(response_items[1], "NOK"))
        fprintf(stdout, "There is no game ongoing...\n");

    else
        fprintf(stderr, "An error occured while attempting to quit.\n");

    return 0;
}