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
void check_guess(char guessed_key[NUM_COLORS_KEY], char secret_key[NUM_COLORS_KEY],int *nw, int *nb);
int check_same_try(char filename[GAME_FILENAME_SIZE], char guess[4]);
int check_same_last_try(char filename[GAME_FILENAME_SIZE], char guess[4]);
int get_game_start_time(char filename[GAME_FILENAME_SIZE], time_t *start_time);
int create_score_file(char filename[GAME_FILENAME_SIZE], char PLID[PLID_SIZE], char secret_key[NUM_COLORS_KEY],
                    int current_try, struct tm *current_time, int duration);
int get_score(int nT, int time);

#endif