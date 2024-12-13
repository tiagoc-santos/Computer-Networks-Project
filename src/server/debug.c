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

int reply_debug(char message_args[ARG_SIZE][CMD_SIZE]){
    return 0;
}