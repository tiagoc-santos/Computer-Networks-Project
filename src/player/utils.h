#ifndef UTILS_H
#define UTILS_H

#define CMD_SIZE 50
#define ARG_SIZE 10
#define MSG_SIZE 50
#define PLID_SIZE 7
#define BUFFER_SIZE 2048
#define SIZE_COLORS 8
#define FILENAME_SIZE 24
#define FIN 1
#define ACT 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

int read_line(char *line);
void split_line(char *line, char arguments[ARG_SIZE][CMD_SIZE]);
int num_args(char arguments[ARG_SIZE][CMD_SIZE]);
int check_digits(char* num_str);

#endif