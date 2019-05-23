#include "char_distribution.h"

int max = 1;
int n = 0;
chardist_t *chardist = NULL;
char *dict_chars = NULL;

// get characters in a file, sorted by frequencies
void get_char_dist(char *dict) {
    chardist = malloc(max * sizeof(chardist_t));

    FILE *fp = fopen(dict, "r");
    char *word = malloc(sizeof(char) * BUFFER_SIZE);
    memset(word, 0, BUFFER_SIZE);

    char c;
    while((c = fgetc(fp)) != EOF) {
        if (c == '\r' || c == '\n') {
            for (int i = 0; i < strlen(word); i++) {
                char ch = word[i];

                // left neighbour
                char l_nb = 0;
                if ((i - 1) >= 0) {
                    l_nb = word[i - 1];
                }
                // right neighbour
                char r_nb = 0;
                if ((i + 1) < strlen(word)) {
                    r_nb = word[i + 1];
                }

                chardist_t *cd = NULL;
                int l_nb_found = 0;
                int r_nb_found = 0;
                for (int j = 0; j < n; j++) {
                    if (chardist[j].c == ch) {
                        cd = &chardist[j];

                        for (int k = 0; k < cd->n_nb; k++) {
                            charfreq_t *nb = &(cd->nb[k]);
                            if (l_nb && nb->c == l_nb) {
                                nb->f++;
                                l_nb_found = 1;
                            }
                            if (r_nb && nb->c == r_nb) {
                                nb->f++;
                                r_nb_found = 1;
                            }
                        }

                        break;
                    }
                }
                if (!cd) {
                    if (n >= max) {
                        max *= 2;
                        chardist = realloc(chardist, max * sizeof(chardist_t));
                    }
                    cd = &chardist[n];
                    cd->c = ch;
                    cd->max_nb = 1;
                    cd->n_nb = 0;
                    cd->nb = malloc(sizeof(charfreq_t));

                    n++;
                }

                if (!l_nb_found || !r_nb_found) {
                    if (cd->n_nb >= (cd->max_nb - 1)) {
                        cd->max_nb *= 2;
                        cd->nb = realloc(cd->nb, cd->max_nb * sizeof(charfreq_t));
                    }
                    if (l_nb && !l_nb_found) {
                        cd->nb[cd->n_nb].c = l_nb;
                        cd->nb[cd->n_nb].f = 1;
                        cd->n_nb++;
                    }
                    if (r_nb && !r_nb_found) {
                        cd->nb[cd->n_nb].c = r_nb;
                        cd->nb[cd->n_nb].f = 1;
                        cd->n_nb++;
                    }
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

    char *dict_chars = malloc(sizeof(char) * (n + 1));
    memset(dict_chars, 0, n + 1);
    for (int i = 0; i < n; i++) {
        dict_chars[i] = chardist[i].c;
    }
}

int check_char_dist(char *guess, int freq) {
    int qualified = 0;
    for (int i = 0; i < (strlen(guess) - 1); i++) {
        for (int j = 0; j < n; j++) {
            if (chardist[j].c == guess[i]) {
                for (int k = 0; k < chardist[j].n_nb; k++) {
                    charfreq_t nb = chardist[j].nb[k];
                    if (nb.c == guess[j] && nb.f >= freq) {
                        qualified = 1;
                        return qualified;
                    }
                }
            }
        }
    }
    return qualified;
}

void free_char_dist() {
    for (int i = 0; i < n; i++) {
        free(chardist[i].nb);
    }
    free(chardist);
}
