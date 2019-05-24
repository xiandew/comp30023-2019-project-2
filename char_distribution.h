/*
 * This module is for counting how often a character appearing next to another
 * character. A MIN_FREQ can be set in main.c. I assume the guess to be good
 * if the frequency is greater than MIN_FREQ.
 */

#ifndef CHAR_DISTRIBUTION_H
#define CHAR_DISTRIBUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048

// ASCII bounds
#define LOWER 32
#define UPPER 126
#define NUM_ASCII UPPER - LOWER + 1

// minimum frequency of a char appearing next to another char for a word to be
// good guess
#define MIN_FREQ 75

typedef int chardist_t[NUM_ASCII][NUM_ASCII];

void get_char_dist(char *dict);
int check_char_dist(char *guess, int len);

extern char dict_chars[BUFFER_SIZE];

#endif
