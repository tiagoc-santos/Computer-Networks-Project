#include "./headers/show_trials.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int show_trials(char PLID[PLID_SIZE]){
    char message[MSG_SIZE], filename[FILENAME_SIZE], file_contents[FILE_SIZE], filepath[BUFFER_SIZE];
    int status, file, bytes_read;
    sprintf(message, "STR %s\n", PLID);
    status = send_tcp_request(message, filename);
    
    if(status == -1)
        return -1;
    else if(status == 0)
        return 0;
    
    fprintf(stdout, "Received trial file named \"%s\" (%d Bytes)\n", filename, status);
    strcpy(filepath, "src/player/scores/");
    strcat(filepath, filename);
    
    // Open the file in read-only mode
    file = open(filepath, O_RDONLY);
    if (file == -1) {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    memset(file_contents, 0, sizeof(file_contents));

    // Read the file character by character and build lines
    while ((bytes_read = read(file, file_contents, FILE_SIZE)) > 0) {
        if (bytes_read == -1){
            fprintf(stderr, "Error reading file\n");
            if (close(file) == -1) {
                fprintf(stderr, "Error closing file\n");
                return -1;
            }
            return -1;
        }
    }
    printf("%s", file_contents);

    if (close(file) == -1) {
        fprintf(stderr, "Error closing file\n");
        return -1;
    }

    return 0;
}
