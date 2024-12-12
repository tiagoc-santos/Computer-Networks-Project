#include "./headers/utils.h"

int read_line(char *line){
    char c;
    int i = 0;

    memset(line, 0, CMD_SIZE);
    while((c = getchar()) != '\n'){
        line[i++] = c;
    }
    line[i] = '\0';
    return 0;
}

void split_line(char *line, char arguments[ARG_SIZE][CMD_SIZE]) {
    char *token;
    int i = 0;

    token = strtok(line, " \t\n");
    while (token != NULL && i < ARG_SIZE) {
        strcpy(arguments[i++], token);
        token = strtok(NULL, " \t\n");
    }

    for (; i < ARG_SIZE; i++) {
        arguments[i][0] = '\0';
    }
}

int num_args(char arguments[ARG_SIZE][CMD_SIZE]){
    int res = 0;
    int i = 0;
    while (i < ARG_SIZE){
        if (arguments[i++][0] == '\0')
            break;
        res++;
    }
    return res;
}

int check_digits(char* num_str){
    int length = strlen(num_str);
    int i;
    for (i = 0; i < length; i++){
        if (!isdigit(num_str[i]))
            return 0;
    }
    return 1;
}