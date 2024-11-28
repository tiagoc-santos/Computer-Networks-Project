#ifndef DEBUG_H
#define DEBUG_H

#include "protocol.h"
#include "utils.h"
#include "try.h"

int validate_colors(char cmd_args[ARG_SIZE][CMD_SIZE]);
int validate_debug(char cmd_args[ARG_SIZE][CMD_SIZE]);
int debug_game(char PLID[PLID_SIZE], int max_playtime, char colors[SIZE_COLORS]);

#endif