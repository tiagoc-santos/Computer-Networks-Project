#include <stdio.h>
#include <string.h>

#define CMD_SIZE 50
#define ARG_SIZE 10

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

int main() {
    char input[CMD_SIZE], arguments[ARG_SIZE][CMD_SIZE];

    while (1) {
        printf("Enter a command: ");

        // Read input line
        char c;
        int i = 0;
        while ((c = getchar()) != '\n' && i < CMD_SIZE - 1) {
            input[i++] = c;
        }
        input[i] = '\0'; // Null-terminate the string

        // Debug: print the raw input
        printf("Raw input: %s\n", input);

        // Split the input into arguments
        split_line(input, arguments);

        // Print the arguments
        for (int i = 0; i < ARG_SIZE && arguments[i][0] != '\0'; i++) {
            printf("Argument %d: %s\n", i, arguments[i]);
        }
        int num;
        num = num_args(arguments);
        printf("%d", num);
        break; // Exit after one iteration for testing purposes
    }

    return 0;
}