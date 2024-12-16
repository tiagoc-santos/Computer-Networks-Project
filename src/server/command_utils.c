#include "./headers/command_utils.h"

int find_specific_game(char PLID[PLID_SIZE], char filename[GAME_FILENAME_SIZE]){
    struct dirent **filelist;
    int n_entries;
    char target_filename[GAME_FILENAME_SIZE];
    char dirname[30];
    int found = 0;

    strcpy(dirname, "src/server/GAMES");
    sprintf(target_filename, "GAME_%s.txt", PLID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);

    if (n_entries <= 0) {
        return 0;
    }

    while (n_entries--) {
        if (!strcmp(filelist[n_entries]->d_name, target_filename)) {
            sprintf(filename, "%s/%s", dirname, filelist[n_entries]->d_name);
            found = 1;
        }
        free(filelist[n_entries]);
    }

    free(filelist);
    return found;
}

int check_timeout(char filename[GAME_FILENAME_SIZE]){
    FILE *file;
    char buffer[BUFFER_SIZE];
    int seconds;
    time_t file_time, current_time;

    file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }

    if(fclose(file) == -1)
        return -1;

    if (sscanf(buffer, "%*s %*s %*s %d %*s %*s %ld", &seconds, &file_time) != 2) {
        return -1;
    }

    current_time = time(NULL);
    
    return(current_time >= file_time + (time_t)seconds);
}

int get_nT(char filename[GAME_FILENAME_SIZE]){
    FILE *file = fopen(filename, "r");
    if (file == 0) {
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Check if the line starts with 'T:'
        if (line[0] == 'T' && line[1] == ':') {
            count++;
        }
    }

    if(fclose(file) == -1)
        return -1;
        
    return count;

}

int move_gamefile(char PLID[PLID_SIZE], struct tm* current_time, char code, char filename[GAME_FILENAME_SIZE]){
    char dirname[40];
    char newfilename[GAME_FILENAME_SIZE];

    sprintf(dirname, "src/server/GAMES/%s", PLID);
    if (mkdir(dirname, 0777) == -1) {
        if (errno != EEXIST) {
            return -1;
        }
    }

    sprintf(newfilename, "%s/%4d%02d%02d_%02d%02d%02d_%c.txt", dirname, current_time->tm_year + 1900,
            current_time->tm_mon+1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
            current_time->tm_sec, code);
    
    if(rename(filename, newfilename) != 0)
        return -1;
    
    return 0;
}

int get_secret_key(char filename[GAME_FILENAME_SIZE], char secret_key[NUM_COLORS_KEY]){
    char buffer[BUFFER_SIZE];
    FILE *file;

    file = fopen(filename, "r");
    if (file == NULL)
        return -1;

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }

    if(sscanf(buffer, "%*s %*s %c%c%c%c %*s %*s %*s %*s", &secret_key[0], 
        &secret_key[1], &secret_key[2], &secret_key[3]) != 4)
        return -1;

    return 0;
}

int add_last_line(char filename[GAME_FILENAME_SIZE], struct tm *current_time, time_t timestamp){
    char lastline[BUFFER_SIZE], buffer[BUFFER_SIZE];
    time_t file_time;
    long int duration, max_time;
    FILE *file;
    
    file = fopen(filename, "r");
    if (file == NULL)
        return -1;

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }

    if (sscanf(buffer, "%*s %*s %*s %ld %*s %*s %ld", &max_time, &file_time) != 2) {
        return -1;
    }

    if(fclose(file) == -1)
        return -1;

    if((time_t)max_time < (timestamp - file_time))
        duration = max_time;
    
    else
        duration = (long int)(timestamp - file_time);
    
    sprintf(lastline, "%4d-%02d-%02d %02d:%02d:%02d %ld\n", current_time->tm_year + 1900,
            current_time->tm_mon+1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
            current_time->tm_sec, duration);
    
    file = fopen(filename, "a");
    if (file == NULL)
        return -1;

    if(fprintf(file, "%s", lastline) < 0){
        return -1;
    }
    
    if(fclose(file) == -1)
        return -1;

    return 0;
}