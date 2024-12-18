#include "./headers/show_trials.h"

int reply_scoreboard(int socket, char message_args[ARG_SIZE][CMD_SIZE]){
    char response[FILE_SIZE];
    SCORELIST score_list;
    int n_scores, i, bytes_written;
    char fname[FILENAME_SIZE];
    char fcontents[FILE_SIZE];

    n_scores = find_top_scores(&score_list);

    if (n_scores == 0){
        strcpy(response, "RSS EMPTY\n");
        send_tcp_message(socket, response, 0, NULL, NULL);
        return 0;
    } 

    srand(time(NULL));
    int random_number = rand() % 10000000;
    sprintf(fname, "TOPSCORES_%07d.txt", random_number);

    char *ptr = fcontents;
    for (i = 0; i < n_scores; i++){
        if ((bytes_written = sprintf(ptr, "%s %c%c%c%c %d\n", score_list.PLID[i], score_list.color_code[i][0],
            score_list.color_code[i][1], score_list.color_code[i][2], score_list.color_code[i][3], score_list.no_tries[i])) < 0){
            strcpy(response, "ERR\n");
            send_tcp_message(socket, response, 0, NULL, NULL);
            return 0;
        }
        ptr += bytes_written;
    }

    int fsize = ptr - fcontents;
    strcpy(response, "RSS OK");
    send_tcp_message(socket, response, fsize, fname , fcontents);

    return 0;
}