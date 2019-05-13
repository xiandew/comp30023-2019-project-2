#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "charfreq.h"

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
int num_cracked = 0;

/*----------------------------------------------------------------------------*/

void read_hashes(char *hshfile);
void check_passwords(char *pwdfile);
void check_guesses(BYTE *guess, int depth, int max_depth, char *charArr);
void check(BYTE *guess);

/*----------------------------------------------------------------------------*/

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

    char *filenames[] = {"common_passwords.txt", "found_pwds.txt"};
    char *common_chars = get_chars(filenames, 2);
    check_guesses(guess, 0, PWD6_LENGTH, common_chars);

    memset(guess, 0, sizeof(guess));
    check_guesses(guess, 0, PWD4_LENGTH, common_chars);

    if (num_cracked < 30) {
        memset(guess, 0, sizeof(guess));
        // brute force for passwords of length 6
        check_guesses(guess, 0, PWD6_LENGTH, NULL);
    }

    free(common_chars);
    free(hashes);
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
    free(password);
}

void check(BYTE *guess) {
    sha256_init(&ctx);
	sha256_update(&ctx, guess, strlen((char *)guess));
    sha256_final(&ctx, hash_buff);
    for (int i = 0; i <= num_hashes; i++) {
        if (memcmp(hash_buff, hashes[i], SHA256_BLOCK_SIZE) == 0) {
            num_cracked++;
            printf("%s %d\n", guess, i + 1);
            break;
        }
    }
}

/**
* Enumerate all guesses in the required ASCII range.
* - guess: grouped characters to be hashed
* - depth: the current depth of the nested loop
* - max_depth: max depth of the nested loop
* - charArr: candidate characters
* Reference: https://stackoverflow.com/questions/19406290
*/
void check_guesses(BYTE *guess, int depth, int max_depth, char *charArr) {
    if(depth == max_depth) {
        if (num_guesses != -1) {
            printf("%s\n", guess);
            if (num_guesses == 0 || ++num_guessed == num_guesses) {
                exit(EXIT_SUCCESS);
            }
        } else {
            check(guess);
        }
    } else {
        if (charArr) {
            for (int i = 0; i < strlen(charArr); i++) {
                guess[depth] = charArr[i];
                check_guesses(guess, depth + 1, max_depth, charArr);
            }
        } else {
            for (guess[depth] = LOWER; guess[depth] <= UPPER; guess[depth]++) {
                check_guesses(guess, depth + 1, max_depth, charArr);
            }
        }
    }
}
