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
