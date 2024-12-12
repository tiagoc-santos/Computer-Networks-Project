#include "./headers/scoreboard.h"

int scoreboard(){
    char message[MSG_SIZE], filename[FILENAME_SIZE], file_contents[FILE_SIZE], filepath[BUFFER_SIZE];
    int status, file, bytes_read;
    strcpy(message, "SSB\n");
    status = send_tcp_request(message, filename);
    
    if(status == -1)
        return -1;

    if(status == 1){
        fprintf(stdout, "Scoreboard is empty.\n");
        return 0;
    }

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