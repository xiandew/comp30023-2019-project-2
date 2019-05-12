#ifndef CHARFREQ_H
#define CHARFREQ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char c;
    int f;
} charfreq_t;

char *get_chars(char **filenames, int numfiles);
int compar(const void *a, const void *b);

#endif
