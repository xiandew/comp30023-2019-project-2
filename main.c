#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

// ASCII bounds
#define LOWER 32
#define UPPER 126

#define PWD4SHA256 "pwd4sha256"
#define PWD4_LENGTH 4

typedef BYTE hash_t[SHA256_BLOCK_SIZE];

SHA256_CTX ctx;
hash_t hash_buff;

// hash values that are read from the file
hash_t *hashes = NULL;
// the max number of hashes that can be put in the array
int max_hashes = 1;
int num_hashes = 0;

// pwd4 or pwd6
int pwd_length;
int num_guesses;

void check(BYTE *guess);
void enum_guesses(BYTE *guess, int depth);


int main(int argc, char **argv) {
    hashes = malloc(max_hashes * sizeof(hash_t));
    FILE *fp = fopen(PWD4SHA256, "r");
    while (1) {
        if (num_hashes >= max_hashes) {
            max_hashes *= 2;
            hashes = realloc(hashes, max_hashes * sizeof(hash_t));
        }
        if (fread(hashes[num_hashes], 1, sizeof(hash_t), fp) <= 0) {
            break;
        }
        num_hashes++;
    }
    if (ferror(fp)) {
        perror("Read file");
    }
    fclose(fp);

    pwd_length = PWD4_LENGTH;
    BYTE guess[pwd_length + 1];
    memset(guess, 0, sizeof(guess));
    enum_guesses(guess, 0);

    return 0;
}

void check(BYTE *guess) {
    sha256_init(&ctx);
	sha256_update(&ctx, guess, strlen((char *)guess));
    sha256_final(&ctx, hash_buff);
    for (int i = 0; i <= num_hashes; i++) {
        if (memcmp(hash_buff, hashes[i], SHA256_BLOCK_SIZE) == 0) {
            printf("%s %d\n", guess, i + 1);
            break;
        }
    }
}

/**
* Enumerate all guesses in the required ASCII range.
* - guess: grouped characters to be hashed
* - depth: the depth of the nested loop
*
* Reference: https://stackoverflow.com/questions/19406290
*/
void enum_guesses(BYTE *guess, int depth) {
    if(depth == pwd_length){
        check(guess);
    } else {
        for (guess[depth] = LOWER; guess[depth] <= UPPER; guess[depth]++) {
            enum_guesses(guess, depth + 1);
        }
    }
}
