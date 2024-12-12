#ifndef START_H
#define START_H

#include "utils.h"
#include "protocol.h"

int validate_start(char cmd_args[ARG_SIZE][CMD_SIZE]);
int start_game(char PLID[PLID_SIZE], char time[ARG_SIZE]);

#endif