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
#define PWD6SHA256 "pwd6sha256"
#define PWD4_LENGTH 4
#define PWD6_LENGTH 6

typedef BYTE hash_t[SHA256_BLOCK_SIZE];

/*----------------------------------------------------------------------------*/

SHA256_CTX ctx;
hash_t hash_buff;

// hash values that are read from the file
hash_t *hashes = NULL;
// the max number of hashes that can be put in the array
int max_hashes = 1;
int num_hashes = 0;

// number of guesses required, specified by argv[1]
int num_guesses = -1;
int num_guessed = 0;

/*----------------------------------------------------------------------------*/

void read_hashes(char *hshfile);
void check_passwords(char *pwdfile);
void enum_guesses(BYTE *guess, int depth, int max_depth, char *charArr);
void check(BYTE *guess);

/*----------------------------------------------------------------------------*/

char *get_chars(char *filename) {
    int max_chars = 1;
    int num_chars = 0;
    char *chars = malloc((max_chars + 1) * sizeof(char));

    FILE *fp = fopen(filename, "r");
    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            continue;
        }
        int found = 0;
        for (int i = 0; i < num_chars; i++) {
            if (chars[i] == c) {
                found = 1;
            }
        }
        if (!found) {
            if (num_chars >= max_chars) {
                max_chars *= 2;
                chars = realloc(chars, (max_chars + 1) * sizeof(char));
            }
            chars[num_chars++] = c;
        }
    }
    chars[num_chars] = 0;
    return chars;
}

int main(int argc, char **argv) {
    if (argc == 3) {
        read_hashes(argv[2]);
        check_passwords(argv[1]);
        return 0;
    }
    if (argc == 2) {
        num_guesses = atoi(argv[1]);
    }

    read_hashes(PWD4SHA256);
    read_hashes(PWD6SHA256);

    BYTE guess[PWD6_LENGTH + 1];
    memset(guess, 0, sizeof(guess));
    // enum_guesses(guess, 0, PWD4_LENGTH);

    char *common_chars = get_chars("common_passwords.txt");
    enum_guesses(guess, 0, PWD6_LENGTH, common_chars);

    return 0;
}

/*----------------------------------------------------------------------------*/

// read hash values in the provided file
void read_hashes(char *hshfile) {
    if (hashes == NULL) {
        hashes = malloc(max_hashes * sizeof(hash_t));
    }
    FILE *fp = fopen(hshfile, "r");
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

// read passwords in the provided file
void check_passwords(char *pwdfile) {
    FILE *fp = fopen(pwdfile, "r");
    char *password = malloc(sizeof(char) * BUFFER_SIZE);
    memset(password, 0, BUFFER_SIZE);

    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            if (strlen(password) == PWD4_LENGTH || strlen(password) == PWD6_LENGTH) {
                check((BYTE *)password);
            }
            memset(password, 0, strlen(password));
            continue;
        }
        if (strlen(password) >= BUFFER_SIZE) {
            password = realloc(password, sizeof(password) * 2);
        }
        password[strlen(password)] = c;
    }
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
    if (num_guesses != -1) {
        printf("%s\n", guess);
        if (++num_guessed == num_guesses) {
            exit(EXIT_SUCCESS);
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
void enum_guesses(BYTE *guess, int depth, int max_depth, char *charArr) {
    if(depth == max_depth) {
        check(guess);
    } else {
        if (charArr) {
            for (int i = 0; i < strlen(charArr); i++) {
                guess[depth] = charArr[i];
                enum_guesses(guess, depth + 1, max_depth, charArr);
            }
        } else {
            for (guess[depth] = LOWER; guess[depth] <= UPPER; guess[depth]++) {
                enum_guesses(guess, depth + 1, max_depth, charArr);
            }
        }
    }
}
