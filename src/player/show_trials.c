#include "show_trials.h"

int show_trials(char PLID[PLID_SIZE]){
    char message[MSG_SIZE];
    int status;
    sprintf(message, "STR %s\n", PLID);
    status = send_tcp_request(message);

    
    return status;
}