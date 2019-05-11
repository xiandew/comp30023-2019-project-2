#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

/*----------------------------------------------------------------------------*/

// ASCII bounds
#define LOWER 32
#define UPPER 126

#define BUFFER_SIZE 2048

#define PWD4SHA256 "pwd4sha256"
#define PWD4_LENGTH 4

typedef BYTE hash_t[SHA256_BLOCK_SIZE];

/*----------------------------------------------------------------------------*/

SHA256_CTX ctx;
hash_t hash_buff;

// hash values that are read from the file
hash_t *hashes = NULL;
// the max number of hashes that can be put in the array
int max_hashes = 1;
int num_hashes = 0;

// number of correct guesses
int num_guessed = 0;
// number of guesses required, specified by argv[1]
int num_guesses = -1;

/*----------------------------------------------------------------------------*/

void read_hashes();
void check_passwords(char *pwdfile);
void enum_guesses(BYTE *guess, int depth, int max_depth);
void check(BYTE *guess);

/*----------------------------------------------------------------------------*/

int main(int argc, char **argv) {
    char *pwdfile = NULL;
    char *hshfile = PWD4SHA256;

    if (argc == 2) {
        num_guesses = atoi(argv[1]);
    }

    if (argc == 3) {
        pwdfile = argv[1];
        hshfile = argv[2];
    }

    read_hashes(hshfile);

    if (pwdfile) {
        check_passwords(pwdfile);
    } else {
        BYTE guess[PWD4_LENGTH + 1];
        memset(guess, 0, sizeof(guess));
        enum_guesses(guess, 0, PWD4_LENGTH);
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

// read passwords in the provided file
void check_passwords(char *pwdfile) {
    FILE *fp = fopen(pwdfile, "r");
    char *password = malloc(sizeof(char) * BUFFER_SIZE);
    memset(password, 0, BUFFER_SIZE);

    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            if (strlen(password)) {
                check((BYTE *)password);
                memset(password, 0, strlen(password));
            }
            continue;
        }
        if (strlen(password) >= BUFFER_SIZE) {
            password = realloc(password, sizeof(password) * 2);
        }
        password[strlen(password)] = c;
    }
}


// read hash values in the provided file
void read_hashes(char *filename) {
    hashes = malloc(max_hashes * sizeof(hash_t));
    FILE *fp = fopen(filename, "r");
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
    fclose(fp);
}

void check(BYTE *guess) {
    sha256_init(&ctx);
	sha256_update(&ctx, guess, strlen((char *)guess));
    sha256_final(&ctx, hash_buff);
    for (int i = 0; i <= num_hashes; i++) {
        if (memcmp(hash_buff, hashes[i], SHA256_BLOCK_SIZE) == 0) {
            printf("%s %d\n", guess, i + 1);
            if (++num_guessed == num_guesses) {
                exit(EXIT_SUCCESS);
            }
            break;
        }
    }
}

/**
* Enumerate all guesses in the required ASCII range.
* - guess: grouped characters to be hashed
* - depth: the current depth of the nested loop
* - max_depth: max depth of the nested loop
* Reference: https://stackoverflow.com/questions/19406290
*/
void enum_guesses(BYTE *guess, int depth, int max_depth) {
    if(depth == max_depth) {
        check(guess);
    } else {
        for (guess[depth] = LOWER; guess[depth] <= UPPER; guess[depth]++) {
            enum_guesses(guess, depth + 1, max_depth);
        }
    }
}
