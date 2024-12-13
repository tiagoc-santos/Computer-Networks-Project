#ifndef TRY_H
#define TRY_H

#include "protocol.h"
#include "../../common/utils.h"
#define GAME_ENDED 1

int is_valid_color(char* color);
int validate_try(char cmd_args[ARG_SIZE][CMD_SIZE]);
int try(char PLID[PLID_SIZE], char c1[CMD_SIZE], char c2[CMD_SIZE], char c3[CMD_SIZE], char c4[CMD_SIZE], int *nT);

#endif