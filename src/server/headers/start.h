#ifndef START_H
#define START_H

#include "protocol.h"
#include "command_utils.h"

int validate_start(char message_args[ARG_SIZE][CMD_SIZE]);
int reply_start(char message_args[ARG_SIZE][CMD_SIZE]);

#endif