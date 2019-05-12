#ifndef PERMUTE_H
#define PERMUTE_H

#include "sha256.h"

void swap(char *x, char *y);
void permute(char *a, int l, int r, void (*check)(BYTE *));

#endif
