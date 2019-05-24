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

typedef struct {
    char c;
    int f;
} charfreq_t;

typedef struct {
    char c;
    // max number of neighbours
    int max_nb;
    // number of neighbours
    int n_nb;
    // neighbours
    charfreq_t *nb;
} chardist_t;

void get_char_dist(char *dict);
int check_char_dist(char *guess, int freq);
void free_char_dist();

extern char *dict_chars;

#endif
