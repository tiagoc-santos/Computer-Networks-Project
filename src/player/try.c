#include "./headers/try.h"

int is_valid_color(char* color){
    return (!strcmp(color, "R") || !strcmp(color, "G") || !strcmp(color, "B") || !strcmp(color, "Y") ||
                !strcmp(color, "O") || !strcmp(color, "P"));
}

int validate_try(char cmd_args[ARG_SIZE][CMD_SIZE]){
    int i;

    for (i = 1; i < 5; i++){
        if (!is_valid_color(cmd_args[i]))
            return -1;
    }
    return 0;
}

int try(char PLID[PLID_SIZE], char c1[CMD_SIZE], char c2[CMD_SIZE], char c3[CMD_SIZE], char c4[CMD_SIZE], int *nT){
    char message[MSG_SIZE], response[MSG_SIZE];
    sprintf(message, "TRY %s %c %c %c %c %d\n", PLID, c1[0], c2[0], c3[0], c4[0], *nT);
    if(send_udp_request(message, strlen(message), player_udp_socket, server_info, response) == -1)
        return -1;
    
    if (!strcmp(response, "RTR ENT\n")){
        fprintf(stdout, "No more attempts\n");
        return GAME_ENDED;
    }
    
    else if (!strcmp(response, "RTR NOK\n")){
        fprintf(stdout, "There is no game ongoing\n");
        return 0;
    }

    else if (!strcmp(response, "RTR INV\n")){
        fprintf(stdout, "Invalid attempt\n");
        return -1;
    }
        
    else if (!strcmp(response, "RTR DUP\n")){
        fprintf(stderr, "You already tried this guess. Number of attempts not increased.\n");
        return 0;
    }
    
    else{
        char response_args[ARG_SIZE][CMD_SIZE];
        split_line(response, response_args);
        if (atoi(response_args[3]) == 4){
            fprintf(stdout, "Congratulations! You guessed the secret key!\n");
            return GAME_ENDED;
        }  
        else{
            fprintf(stdout, "Number of correct colors in correct position: %d\n", atoi(response_args[3]));
            fprintf(stdout, "Number of correct colors in wrong position: %d\n", atoi(response_args[4]));
            *nT += 1;
            return 0;
        }
    }
}
