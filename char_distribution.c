#include "char_distribution.h"

chardist_t char_dist;
char dict_chars[BUFFER_SIZE];

// get characters in a file, sorted by frequencies
void get_char_dist(char *dict) {
    memset(char_dist, 0, NUM_ASCII * NUM_ASCII * sizeof(int));

    FILE *fp = fopen(dict, "r");
    char *word = malloc(sizeof(char) * BUFFER_SIZE);
    memset(word, 0, BUFFER_SIZE);

    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            for (int i = 0; i < strlen(word); i++) {
                char ch = word[i];

                // right neighbours
                char r_nb = 0;
                if ((i + 1) < strlen(word)) {
                    r_nb = word[i + 1];
                }

                if (r_nb) {
                    char_dist[ch - LOWER][r_nb - LOWER]++;
                }
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

    int n = 0;
    for (int i = 0; i <= NUM_ASCII; i++) {
        int isNotEmpty = 0;
        for (int j = 0; j <= NUM_ASCII; j++) {
            isNotEmpty = isNotEmpty | char_dist[i][j];
        }
        if (isNotEmpty) {
            dict_chars[n++] = i + LOWER;
        }
    }
    dict_chars[n] = 0;
}

int check_char_dist(char *guess, int len) {
    int is_good = 1;

    for (int i = 0; i < (len - 1); i++) {
        char ch = guess[i];
        char r_nb = guess[i + 1];
        if (char_dist[ch - LOWER][r_nb - LOWER] < MIN_FREQ) {
            is_good = 0;
            break;
        }
    }
    return is_good;
}
