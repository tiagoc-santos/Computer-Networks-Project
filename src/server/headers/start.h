#ifndef START_H
#define START_H

#include "protocol.h"
#include <stdlib.h>
#include <time.h>

extern int game_running;
extern int nT;
extern char secret_key[NUM_COLORS_KEY];

int validate_start(char message_args[ARG_SIZE][CMD_SIZE]);
int reply_start(char message_args[ARG_SIZE][CMD_SIZE]);

#endif