#ifndef DEBUG_H
#define DEBUG_H

#include "protocol.h"
#include "../../common/utils.h"
#include "try.h"
#include "command_utils.h"

int reply_debug(char message_args[ARG_SIZE][CMD_SIZE]);
int validate_colors(char cmd_args[ARG_SIZE][CMD_SIZE]);
int validate_debug(char cmd_args[ARG_SIZE][CMD_SIZE]);

#endif