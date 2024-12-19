#include "./headers/command_utils.h"

int find_active_game(char PLID[PLID_SIZE], char filename[GAME_FILENAME_SIZE]){
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
    
    if (file == NULL) {
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
        &secret_key[1], &secret_key[2], &secret_key[3]) != 4){
        fclose(file);
        return -1;
    
    }
    
    if(fclose(file) == -1)
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
        fclose(file);
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
        fclose(file);
        return -1;
    }
    
    if(fclose(file) == -1)
        return -1;

    return 0;
}

int check_same_try(char filename[GAME_FILENAME_SIZE], char guess[4]){
    FILE *file = fopen(filename, "r");
    char recorded_guess[NUM_COLORS_KEY + 1];
    
    if (file == NULL) {
        return -1;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Check if the line starts with 'T:'
        if (line[0] == 'T' && line[1] == ':') {
            if(sscanf(line, "%*s %s %*s %*s %*s", recorded_guess) != 1)
                return -1;
            if(!strcmp(recorded_guess, guess))
                return 1;
        }
    }

    if(fclose(file) == -1)
        return -1;

    return 0;
}

int check_same_last_try(char filename[GAME_FILENAME_SIZE], char guess[4]){
    FILE *file = fopen(filename, "r");
    char recorded_guess[NUM_COLORS_KEY + 1];
    
    if (file == NULL) {
        return -1;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Check if the line starts with 'T:'
        if (line[0] == 'T' && line[1] == ':') {
            if(sscanf(line, "%*s %s %*s %*s %*s", recorded_guess) != 1)
                return -1;
        }
    }

    if(fclose(file) == -1)
        return -1;

    return !strcmp(recorded_guess, guess);
}

int read_tries(char status, char filename[GAME_FILENAME_SIZE], char fcontents[FILE_SIZE], int *fsize){
    FILE *file = fopen(filename, "r");
    int bytes_written, nB, nW, remaining_time;
    char guess_code[NUM_COLORS_KEY];
    time_t start_time, current_time;
    int last_line = 0, duration, max_time;
    
    if (file == NULL) {
        return -1;
    }

    char line[256];

    fgets(line, sizeof(line), file);
    if(sscanf(line, "%*s %*s %*s %d %*s %*s %ld", &max_time, &start_time) != 2){
        fclose(file);
        return -1;
    }

    char *ptr = fcontents;
    while (fgets(line, sizeof(line), file)) {
        // Check if the line starts with 'T:'
        if (line[0] == 'T' && line[1] == ':') {
            last_line = 1;
            if(sscanf(line, "%*s %s %d %d %*s", guess_code, &nB, &nW) != 3){
                fclose(file);
                return -1;
            }
            if ((bytes_written = sprintf(ptr, "T: %c %c %c %c nB: %d nW: %d\n", guess_code[0],
                    guess_code[1], guess_code[2], guess_code[3], nB, nW)) < 0){
                fclose(file);
                return -1;
            }
            ptr += bytes_written;
        }
        else if (last_line == 1){
            if(sscanf(line, "%*s %*s %d", &duration) != 1){
                fclose(file);
                return -1;
            }
            break;
        }
    }

    if(status == 'A'){
        time(&current_time);
        remaining_time = max_time - (int)(current_time - start_time);
        if ((bytes_written = sprintf(ptr, "You still have %d seconds to guess the secret key!\n", remaining_time)) < 0){
            fprintf(stderr, "Error writing to buffer.\n");
            fclose(file);
            return -1;
        }
    }
    else if(status == 'F'){
        if (last_line == 0){
            if ((bytes_written = sprintf(ptr, "No attempts were made.\n")) < 0){
                fprintf(stderr, "Error writing to buffer.\n");
                fclose(file);
                return -1;
            }
        }
        else{
            if ((bytes_written = sprintf(ptr, "The game lasted %d seconds.\n", duration)) < 0){
                fprintf(stderr, "Error writing to buffer.\n");
                fclose(file);
                return -1;
            }
        } 
    }

    ptr += bytes_written;
    *fsize = ptr - fcontents;

    if(fclose(file) == -1)
        return -1;

    return 0;
}

void check_guess(char guessed_key[NUM_COLORS_KEY], char secret_key[NUM_COLORS_KEY], int *nB, int *nW){
    int blacks = 0, count = 0;
    int key[6] = {0,0,0,0,0,0};
    int guess[6] = {0,0,0,0,0,0};
    int i;
    // count number of blacks
    for(i = 0; i < 4; i++){
        if(guessed_key[i] == secret_key[i])
            blacks += 1;
    }

    for (i = 0; i < 4; i++){
        switch (guessed_key[i]){
            case 'R':
                guess[0]+=1;
                break;
            case 'G':
                guess[1]+=1;
                break;
            case 'B':
                guess[2]+=1;
                break;
            case 'Y':
                guess[3]+=1;
                break;
            case 'O':
                guess[4]+=1;
                break;
            case 'P':
                guess[5]+=1;
                break;
        }          
        switch (secret_key[i]){
            case 'R':
                key[0]+=1;
                break;
            case 'G':
                key[1]+=1;
                break;
            case 'B':
                key[2]+=1;
                break;
            case 'Y':
                key[3]+=1;
                break;
            case 'O':
                key[4]+=1;
                break;
            case 'P':
                key[5]+=1;
                break;
        }
    }
    for (i = 0; i < 6; i++){
        if (guess[i] <= key[i])
            count += guess[i];
        else
            count += key[i];
    }
    *nW = count - blacks;
    *nB = blacks;
}

int get_game_start_time(char filename[GAME_FILENAME_SIZE], time_t *start_time){
    char line[256];
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -1;
    }

    if(fclose(file) == -1)
        return -1;

    if(sscanf(line, "%*s %*s %*s %*s %*s %*s %ld", start_time) != 1)
        return -1;
    
    return 0;
}

int create_score_file(char filename[GAME_FILENAME_SIZE], char PLID[PLID_SIZE], char secret_key[NUM_COLORS_KEY], 
                    int current_try, struct tm *current_time, int duration){
    char dirname[40];
    char newfilename[GAME_FILENAME_SIZE];
    char newline[BUFFER_SIZE], buffer[256];
    char mode[6], M;
    int score;

    strcpy(dirname, "src/server/SCORES");

    score = get_score(current_try, duration);

    sprintf(newfilename, "%s/%03d_%s_%4d%02d%02d_%02d%02d%02d.txt", dirname, score, PLID, current_time->tm_mday,
            current_time->tm_mon+1, current_time->tm_year + 1900, current_time->tm_hour, current_time->tm_min,
            current_time->tm_sec);
    
    FILE *game_file = fopen(filename, "r");
    
    if(game_file == NULL)
        return -1;

    if(fgets(buffer, sizeof(buffer), game_file) == NULL)
        return -1;

    if(sscanf(buffer, "%*s %c %*s %*s %*s %*s %*s", &M) != 1){
        fclose(game_file);
        return -1;
    }

    if(fclose(game_file) == -1)
        return -1;

    FILE *score_file = fopen(newfilename, "w");
    if(score_file == NULL)
        return -1;

    if(M == MODE_PLAY){
        strcpy(mode, "PLAY");
        mode[4] = '\0';
    }
    else if (M == MODE_DEBUG){
        strcpy(mode, "DEBUG");
        mode[5] = '\0';
    }
        

    sprintf(newline, "%03d %s %c%c%c%c %d %s\n", score, PLID, secret_key[0],
        secret_key[1], secret_key[2], secret_key[3], current_try, mode);
    
    if(fprintf(score_file, "%s", newline) <= 0){
        fclose(score_file);
        return -1;
    }

    if(fclose(score_file) == -1)
        return -1;
    
    return 0;
}

// Returns a score between 0 and 100
int get_score(int nT, int time){
    double nT_d = (double) nT;
    double time_d = (double) time;
    int score = (int) (20.0 * ((600.0-time_d)/600.0) + 80.0 * ((8.0-nT_d+1.0)/8.0));
    return score;
}

int find_last_game(char PLID[PLID_SIZE], char fname[GAME_FILENAME_SIZE]){
    struct dirent **filelist;
    int n_entries, found;
    char dirname[30];

    sprintf(dirname, "src/server/GAMES/%s", PLID);
    n_entries = scandir(dirname, &filelist, 0 , alphasort);
    found = 0;

    if (n_entries <= 0)
        return 0;
    else{
        while (n_entries--){
            if (filelist[n_entries]->d_name[0] != '.' && !found){
                sprintf(fname, "src/server/GAMES/%s/%s", PLID, filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
        }
        free(filelist);
    }
    return found;
}

int find_top_scores(SCORELIST *list){
    struct dirent **filelist;
    int n_entries, i_file;
    char fname[GAME_FILENAME_SIZE];
    FILE *fp;
    char mode[MODE_CHAR_SIZE];

    n_entries = scandir("src/server/SCORES", &filelist, 0, alphasort);

    if (n_entries <= 0)
        return 0;
    else{
        i_file = 0;
        while (n_entries--){
            if (filelist[n_entries]->d_name[0] != '.' && i_file < 10){
                sprintf(fname, "src/server/SCORES/%s", filelist[n_entries]->d_name);
                fp = fopen(fname, "r");
                if (fp == NULL)
                    fprintf(stderr, "Error opening file\n");
                else{
                    fscanf(fp, "%d %s %s %d %s", &list->score[i_file], list->PLID[i_file], list->color_code[i_file],
                            &list->no_tries[i_file], mode);
                    
                    if(!strcmp(mode, "PLAY"))
                        list->mode[i_file] = MODE_PLAY;
                    else if(!strcmp(mode, "DEBUG"))
                        list->mode[i_file] = MODE_DEBUG;

                    if(fclose(fp) == -1){
                        fprintf(stderr, "Error closing file\n");
                        free(filelist[n_entries]);
                        free(filelist);
                        return -1;
                    }
                    i_file++;
                }
            }
            free(filelist[n_entries]);
        }
        free(filelist);
    }
    list->n_scores = i_file;
    return(i_file);
}
