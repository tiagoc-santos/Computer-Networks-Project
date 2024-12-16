#ifndef QUIT_H
#define QUIT_H

#include "protocol.h"
#include "../../common/utils.h"
#include "command_utils.h"

#define ANSWER_SIZE 256

int reply_quit(char message_args[ARG_SIZE][CMD_SIZE]);
int validate_quit(char message_args[ARG_SIZE][CMD_SIZE]);

#endif