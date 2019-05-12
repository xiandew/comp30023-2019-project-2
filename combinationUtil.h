#ifndef COMBINATIONUTIL_H
#define COMBINATIONUTIL_H

#include "sha256.h"

void combinationUtil(char arr[], char data[], int start, int end,
					int index, int r, void (*check)(BYTE *));
void checkCombination(char arr[], int n, int r, void (*check)(BYTE *));


#endif
