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

int reply_try(char message_args[ARG_SIZE][CMD_SIZE]){
    int nw, nb;
    return 0;
}

int check_nwhite_nblack(char guessed_key[NUM_COLORS_KEY], int *nw, int *nb){
    
}