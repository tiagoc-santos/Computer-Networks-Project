#include "scoreboard.h"

int scoreboard(){
    char message[MSG_SIZE];
    int status;

    strcpy(message, "SSB\n");
    status = send_tcp_request(message);

    return 0;
}