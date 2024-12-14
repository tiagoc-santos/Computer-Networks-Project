#include "./headers/start.h"

int game_running = 0;
int nT = 0;
char secret_key[NUM_COLORS_KEY];

int validate_start(char message_args[ARG_SIZE][CMD_SIZE]) {
    return (num_args(message_args) != 3 || strlen(message_args[1]) != 6 || !check_digits(message_args[1])
                || !check_digits(message_args[2]) || atoi(message_args[2]) > 600 || atoi(message_args[2]) < 1);
}

int reply_start(char message_args[ARG_SIZE][CMD_SIZE]){
    char response[MSG_SIZE];
    if (validate_start(message_args) != 0){
        strcpy(response, "RSG ERR\n");
        send_message_udp(response);
        return 0;
    }

    if(game_running && nT > 0){
        strcpy(response, "RSG NOK\n");
        send_message_udp(response);
        return 0;
    }

    //initialize random secret key
    char* possible_colors = "RGBYOP";
    int num_colors =  strlen(possible_colors);
    
    srand(time(NULL));
    for (int i = 0; i < 4; i++) {
        int random_index = rand() % num_colors;
        secret_key[i] = possible_colors[random_index];
    }

    strcpy(response, "RSG OK\n");
    send_message_udp(response);
    
    return 0;
}

