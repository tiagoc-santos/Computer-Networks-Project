#include "./headers/start.h"

int validate_start(char message_args[ARG_SIZE][CMD_SIZE]) {
    return (num_args(message_args) != 3 || strlen(message_args[1]) != 6 || !check_digits(message_args[1])
                || !check_digits(message_args[2]) || atoi(message_args[2]) > 600 || atoi(message_args[2]) < 1);
}

int reply_start(char message_args[ARG_SIZE][CMD_SIZE]){
    char response[MSG_SIZE];
    if (validate_start != 0){
        strcpy(response, "RSG ERR\n");
        send_message_udp(response);
    }
    return 0;
}

