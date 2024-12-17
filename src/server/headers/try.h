#ifndef TRY_H
#define TRY_H

#include "protocol.h"
#include "command_utils.h"

int reply_try(char message_args[ARG_SIZE][CMD_SIZE]);
int is_valid_color(char* color);

#endif