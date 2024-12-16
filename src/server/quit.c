#include "./headers/quit.h"

int validate_quit(char message_args[ARG_SIZE][CMD_SIZE]){
    return (num_args(message_args) != 2 || strlen(message_args[1]) != 6 || !check_digits(message_args[1]));
}

int reply_quit(char message_args[ARG_SIZE][CMD_SIZE]){
    char filename[GAME_FILENAME_SIZE], response[MSG_SIZE], PLID[PLID_SIZE];
    char code, secret_key[NUM_COLORS_KEY];
    struct tm *current_time;
    time_t timestamp;

    if(validate_quit(message_args) != 0){
        strcpy(response, "RQT ERR\n");
        send_message_udp(response);
        return 0;
    }

    strcpy(PLID, message_args[1]);
    PLID[6] = '\0';

    //The player has no active game
    if(!find_specific_game(PLID, filename)){
        strcpy(response, "RQT NOK\n");
        send_message_udp(response);
        return 0;
    }

    //Checks if the time ran out in the ongoing game
    int timeout = check_timeout(filename);
    if(timeout == -1){
        strcpy(response, "ERR\n");
        send_message_udp(response);
        return 0;
    }
    
    if(timeout == 1)
        code = 'T';
    else
        code = 'Q';

    time(&timestamp);
    current_time = gmtime(&timestamp);

    //
    if(add_last_line(filename, current_time, timestamp) != 0){
        strcpy(response, "ERR\n");
        send_message_udp(response);
        return 0;
    }

    // Get games's secret key
    if(get_secret_key(filename, secret_key) != 0){
        strcpy(response, "RQT ERR\n");
        send_message_udp(response);
        return 0;
    }

    sprintf(response, "RTQ OK %c %c %c %c\n", secret_key[0], secret_key[1], secret_key[2], secret_key[3]);
    send_message_udp(response);

    // Move gamefile to directory GAMES/PLID
    if(move_gamefile(PLID, current_time, code, filename) != 0){
        strcpy(response, "RQT ERR\n");
        send_message_udp(response);
        return 0;
    }

    return 0;
}