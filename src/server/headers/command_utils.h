#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H

#include "protocol.h"
#include "../../common/utils.h"
#include <stdlib.h>
#include <time.h>
#include <dirent.h>

int find_specific_game(char PLID[PLID_SIZE], char filename[GAME_FILENAME_SIZE]);
int check_timeout(char filename[GAME_FILENAME_SIZE]);
int get_nT(char filename[GAME_FILENAME_SIZE]);
int move_gamefile(char PLID[PLID_SIZE], struct tm* current_time, char code, char filename[GAME_FILENAME_SIZE]);
int get_secret_key(char filename[GAME_FILENAME_SIZE], char secret_key[NUM_COLORS_KEY]);
int add_last_line(char filename[GAME_FILENAME_SIZE], struct tm *current_time, time_t timestamp);

#endif