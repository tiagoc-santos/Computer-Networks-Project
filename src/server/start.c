#include "./headers/start.h"

char secret_key[NUM_COLORS_KEY];

int validate_start(char message_args[ARG_SIZE][CMD_SIZE]) {
    return (num_args(message_args) != 3 || strlen(message_args[1]) != 6 || !check_digits(message_args[1])
                || !check_digits(message_args[2]) || atoi(message_args[2]) > 600 || atoi(message_args[2]) < 1);
}

int reply_start(char message_args[ARG_SIZE][CMD_SIZE]){
    char response[MSG_SIZE], filename[GAME_FILENAME_SIZE];
    char PLID[PLID_SIZE], secret_key[NUM_COLORS_KEY];
    struct tm *current_time;
    time_t fulltime;

    //Checks if the start arguments are correct
    if (validate_start(message_args) != 0){
        strcpy(response, "RSG ERR\n");
        send_message_udp(response);
        return 0;
    }

    strcpy(PLID, message_args[1]);
    PLID[6] = '\0';
    
    //Checks if the player has an ongoing game
    if(find_specific_game(PLID, filename) == 1){
        int timeout = check_timeout(filename);
        if(timeout == -1){
            strcpy(response, "ERR\n");
            send_message_udp(response);
            return 0;
        }
        // if the game has ended due to running out of time moves the file to GAMES/PLID
        else if(timeout == 1){
            time(&fulltime);
            current_time = gmtime(&fulltime);
            if (add_last_line(filename, current_time, fulltime) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }

            if(move_gamefile(PLID, current_time, 'T', filename) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
        }
        
        //The game is still playable timewise
        else {
            int nT = get_nT(filename);
            if (nT == -1){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
            //If the player has done atleast a try
            else if(nT > 0){
                strcpy(response, "RSG NOK\n");
                send_message_udp(response);
                return 0;
            }
        }
    }
    
    //initialize random secret key
    char* possible_colors = "RGBYOP";
    int num_colors =  strlen(possible_colors);
    
    srand(time(NULL));
    for (int i = 0; i < 4; i++) {
        int random_index = rand() % num_colors;
        secret_key[i] = possible_colors[random_index];
    }

    //Create game file
    sprintf(filename, "src/server/GAMES/GAME_%s.txt", PLID);
    int game_file = open(filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if(game_file == -1){
        strcpy(response, "ERR\n");
        send_message_udp(response);
        return 0;
    }

    char firstline[BUFFER_SIZE];
    time(&fulltime);
    current_time = gmtime(&fulltime); 
    sprintf(firstline, "%s P %c%c%c%c %s %4d-%02d-%02d %02d:%02d:%02d %ld\n", PLID, secret_key[0], 
            secret_key[1], secret_key[2], secret_key[3], message_args[2], current_time->tm_year + 1900,
            current_time->tm_mon+1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
            current_time->tm_sec, fulltime);

    int nbytes = strlen(firstline), nwritten;
    int nleft = nbytes;
    char buffer[BUFFER_SIZE];
    char *ptr = strcpy(buffer, firstline);
    
    while(nleft > 0){
        nwritten = write(game_file,ptr,nleft);
        if(nwritten == -1){
            strcpy(response, "ERR\n");
            send_message_udp(response);
            close(game_file);
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    strcpy(response, "RSG OK\n");
    send_message_udp(response);
    
    return 0;
}

