#include "./headers/try.h"

int is_valid_color(char* color){
    return (!strcmp(color, "R") || !strcmp(color, "G") || !strcmp(color, "B") || !strcmp(color, "Y") ||
                !strcmp(color, "O") || !strcmp(color, "P"));
}

int validate_try(char message_args[ARG_SIZE][CMD_SIZE]){
    int i;
    if (num_args(message_args) != 7 || strlen(message_args[1]) != 6 || !check_digits(message_args[1]) || !check_digits(message_args[6]))
        return -1;

    for (i = 2; i < 6; i++) {
        if (!is_valid_color(message_args[i]))
            return -1;
    }
    return 0;
}

int reply_try(char message_args[ARG_SIZE][CMD_SIZE]){
    char response[MSG_SIZE], PLID[PLID_SIZE], filename[GAME_FILENAME_SIZE];
    char guess[NUM_COLORS_KEY+1], secret_key[NUM_COLORS_KEY];
    int nB, nW;
    struct tm *current_time;
    time_t fulltime, timestamp;
    

    if(validate_try(message_args) != 0){
        strcpy(response, "RTR ERR\n");
        send_message_udp(response);
        return 0;
    }
    
    int current_try = atoi(message_args[6]);
    strcpy(PLID, message_args[1]);
    PLID[6] = '\0';

    //Checks if the player has an ongoing game
    if(find_active_game(PLID, filename) == 1){
        int timeout = check_timeout(filename);
        if(timeout == -1){
            strcpy(response, "ERR\n");
            send_message_udp(response);
            return 0;
        }

        time(&fulltime);
        current_time = gmtime(&fulltime);

        // if the game has ended due to running out of time moves the file to GAMES/PLID
        if(timeout == 1){
            if (add_last_line(filename, current_time, fulltime) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
            
            if(get_secret_key(filename, secret_key) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
            
            if(move_gamefile(PLID, current_time, 'T', filename) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }

            sprintf(response, "RTR ETM %c %c %c %c\n", secret_key[0], secret_key[1], secret_key[2], secret_key[3]);
            send_message_udp(response);
            return 0;
        }
        
        //The game is still playable timewise
        else {
            int nT = get_nT(filename);
            if (nT == -1){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }

            sprintf(guess, "%c%c%c%c", message_args[2][0], message_args[3][0], message_args[4][0], message_args[5][0]);

            //invalid attempt - not the expected try number or the expected - 1 but different from the last try (not a resend)
            if((current_try != nT + 1) || (current_try == nT && !check_same_last_try(filename, guess))){
                strcpy(response, "RTR INV\n");
                send_message_udp(response);
                return 0;
            }

            //same attempt as one of the previous ones
            if(check_same_try(filename, guess)){
                strcpy(response, "RTR DUP\n");
                send_message_udp(response);
                return 0;
            }

            if(get_secret_key(filename, secret_key) != 0){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }

            //failed last attempt. Game ends on a loss
            if (current_try == 8 && strcmp(secret_key, guess)){
                char code = 'F';
                
                time(&timestamp);
                current_time = gmtime(&timestamp);
                
                // Add line with info regarding game ending
                if(add_last_line(filename, current_time, timestamp) != 0){
                    strcpy(response, "ERR\n");
                    send_message_udp(response);
                    return 0;
                }

                // Move gamefile to directory GAMES/PLID
                if(move_gamefile(PLID, current_time, code, filename) != 0){
                    strcpy(response, "ERR\n");
                    send_message_udp(response);
                    return 0;
                }

                sprintf(response, "RTR ENT %c %c %c %c\n", secret_key[0], secret_key[1], secret_key[2], secret_key[3]);
                send_message_udp(response);
                return 0;
            }

            check_guess(guess, secret_key, &nB, &nW);
            if(current_try == nT && check_same_last_try(filename, guess)){
                sprintf(response, "RTR OK %d %d %d\n", nT, nB, nW);
                send_message_udp(response);
                return 0;
            }

            FILE *game_file = fopen(filename, "a");
            char newline[BUFFER_SIZE];
            time_t start_time = 0;

            if(get_game_start_time(filename, &start_time) == -1){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
            
            int duration = (int)(fulltime - start_time);

            sprintf(newline, "T: %s %d %d %d\n", guess, nB, nW, duration);
            if(fprintf(game_file, "%s", newline) <= 0){
                fclose(game_file);
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }
            
            if(fclose(game_file) == -1){
                strcpy(response, "ERR\n");
                send_message_udp(response);
                return 0;
            }

            // Player guessed correctly
            if(nB == 4){
                if(add_last_line(filename, current_time, fulltime) != 0){
                    strcpy(response, "ERR\n");
                    send_message_udp(response);
                    return 0;
                }
                
                if(create_score_file(filename, PLID, secret_key, current_try, current_time, duration) != 0){
                    strcpy(response, "ERR\n");
                    send_message_udp(response);
                    return 0;
                }
                
                if(move_gamefile(PLID, current_time, 'W', filename) != 0){
                    strcpy(response, "ERR\n");
                    send_message_udp(response);
                    return 0;
                }
            }

            sprintf(response, "RTR OK %d %d %d\n", nT, nB, nW);
            send_message_udp(response);

            return 0;
        }
    }

    // No game running
    else{
        strcpy(response, "RTR NOK\n");
        send_message_udp(response);
        return 0;
    }
    
    return 0;
}