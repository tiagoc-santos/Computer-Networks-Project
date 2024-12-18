#include "./headers/show_trials.h"

int validate_st(char message_args[ARG_SIZE][CMD_SIZE]){
    return (num_args(message_args) != 2 || strlen(message_args[1]) != 6 || !check_digits(message_args[1]));
}

int reply_showtrials(int socket, char message_args[ARG_SIZE][CMD_SIZE]){
    char response[MSG_SIZE], PLID[PLID_SIZE], fcontents[FILE_SIZE];
    char agfilename[GAME_FILENAME_SIZE], fgfilename[GAME_FILENAME_SIZE];
    int active_game = 0, finished_game = 0, fsize;
    
    // Checks if the arguments are correct
    if(validate_st(message_args) != 0){
        strcpy(response, "ERR3\n");
        send_tcp_message(socket, response, 0, NULL, NULL);
        return 0;
    }

    strcpy(PLID, message_args[1]);
    PLID[PLID_SIZE] = '\0';

    active_game = find_active_game(PLID, agfilename);
    finished_game = find_last_game(PLID, fgfilename);

    // Check if player has active game
    if(!active_game && !finished_game){
        strcpy(response, "RST NOK\n");
        send_tcp_message(socket, response, 0, NULL, NULL);
        return 0;
    }

    char fname[FILENAME_SIZE];
    sprintf(fname, "STATE_%s.txt", PLID);
    if(active_game){
        if (read_tries('A', agfilename, fcontents, &fsize) == -1){
            strcpy(response, "ERR1\n");
            send_tcp_message(socket, response, 0, NULL, NULL);
            return 0;
        }
        strcpy(response, "RST ACT");
        send_tcp_message(socket, response, fsize, fname, fcontents);
    }
    else if (finished_game){
        if (read_tries('F', fgfilename, fcontents, &fsize) == -1){
            strcpy(response, "ERR2\n");
            send_tcp_message(socket, response, 0, NULL, NULL);
            return 0;
        }
        strcpy(response, "RST FIN");
        send_tcp_message(socket, response, fsize, fname, fcontents);
        return 0;
    }


    return 0;
}