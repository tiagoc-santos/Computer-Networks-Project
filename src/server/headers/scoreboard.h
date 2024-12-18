#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "protocol.h"
#include "command_utils.h"

int reply_scoreboard(int socket, char message_args[ARG_SIZE][CMD_SIZE]);

#endif