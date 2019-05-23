#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "char_distribution.h"

/*----------------------------------------------------------------------------*/

// ASCII bounds
#define LOWER 32
#define UPPER 126

#define PWD4SHA256 "pwd4sha256"
#define PWD6SHA256 "pwd6sha256"
#define PWD4_LENGTH 4
#define PWD6_LENGTH 6

#define DICTIONARY "common_passwords.txt"

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
void dictionary_search(char *dict, int len);
void small_range_search(char *dict);
void full_range_search();
void full_range_check(BYTE *guess, int depth, int max_depth);
void check(BYTE *guess);

/*----------------------------------------------------------------------------*/

int main(int argc, char **argv) {
    if (argc == 3) {
        read_hashes(argv[2]);
        dictionary_search(argv[1], -1);
        return 0;
    }
    if (argc == 2) {
        num_guesses = atoi(argv[1]);
    }

    free_char_dist(get_char_dist(DICTIONARY));
    return 0;

    // read_hashes(PWD4SHA256);
    // read_hashes(PWD6SHA256);
    //
    // // search for passwords of 6 length
    // // dictionary search
    // dictionary_search(DICTIONARY, 6);
    //
    // // small range exhaustive search
    // small_range_search(DICTIONARY);
    //
    // // full range exhaustive search
    // memset(guess, 0, sizeof(guess));
    // check_guesses(guess, 0, PWD6_LENGTH, NULL);
    //
    // // search for passwords of 4 length
    // // full range exhaustive search
    // memset(guess, 0, sizeof(guess));
    // check_guesses(guess, 0, PWD4_LENGTH, NULL);
    //
    // free(common_chars);
    // free(hashes);
    // return 0;
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
void dictionary_search(char *dict, int len) {
    FILE *fp = fopen(dict, "r");
    char *word = malloc(sizeof(char) * BUFFER_SIZE);
    memset(word, 0, BUFFER_SIZE);

    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            if (len >= 0 && strlen(word) == len) {
                check((BYTE *)word);
            }
            memset(word, 0, strlen(word));
            continue;
        }
        if (strlen(word) >= BUFFER_SIZE) {
            word = realloc(word, sizeof(word) * 2);
        }
        word[strlen(word)] = c;
    }
    free(word);
}

void small_range_search(char *dict) {
    // BYTE guess[PWD6_LENGTH + 1];
    // memset(guess, 0, sizeof(guess));
    //
    // char *common_chars = get_chars(DICTIONARY);
    // check_guesses(guess, 0, PWD6_LENGTH, common_chars);
}

void small_range_check(BYTE *guess, int depth, int max_depth, char *charArr) {
    // if(depth == max_depth) {
    //     check(guess);
    // } else {
    //     for (int i = 0; i < strlen(charArr); i++) {
    //         guess[depth] = charArr[i];
    //         check_guesses(guess, depth + 1, max_depth, charArr);
    //     }
    // }
}

/**
* Enumerate all guesses in the required ASCII range (32~126).
* - guess: grouped characters to be hashed
* - depth: the current depth of the nested loop
* - max_depth: max depth of the nested loop
* Reference: https://stackoverflow.com/questions/19406290
*/
void full_range_check(BYTE *guess, int depth, int max_depth) {
    if(depth == max_depth) {
        check(guess);
    } else {
        for (guess[depth] = LOWER; guess[depth] <= UPPER; guess[depth]++) {
            full_range_check(guess, depth + 1, max_depth);
        }
    }
}

void check(BYTE *guess) {
    if (num_guesses != -1) {
        printf("%s\n", guess);
        if (num_guesses == 0 || ++num_guessed == num_guesses) {
            exit(EXIT_SUCCESS);
        }
        return;
    }
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
