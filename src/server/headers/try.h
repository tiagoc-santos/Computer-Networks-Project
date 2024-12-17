#ifndef TRY_H
#define TRY_H

#include "protocol.h"

int reply_quit(char message_args[ARG_SIZE][CMD_SIZE]);
int is_valid_color(char* color);
int check_nwhite_nblack(char guessed_key[NUM_COLORS_KEY], int nw, int nb);

#endif