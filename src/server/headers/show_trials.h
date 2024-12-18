#ifndef SHOWTRIALS_H
#define SHOWTRIALS_H

#include "protocol.h"
#include "command_utils.h"

int reply_showtrials(int socket, char message_args[ARG_SIZE][CMD_SIZE]);
int validate_st(char message_args[ARG_SIZE][CMD_SIZE]);

#endif