#include "charfreq.h"

charfreq_t *charfreqs = NULL;

// get characters in a file, sorted by frequencies
char *get_chars(char **filenames, int numfiles) {
    int size = 1;
    int n = 0;
    charfreqs = malloc(size * sizeof(charfreq_t));

    for (int i = 0; i < numfiles; i++) {
        FILE *fp = fopen(filenames[i], "r");
        char c;
        while((c = fgetc(fp)) != EOF) {
            if (c == '\r' || c == '\n') {
                continue;
            }
            int found = 0;
            for (int i = 0; i < n; i++) {
                if (charfreqs[i].c == c) {
                    found = 1;
                    charfreqs[i].f++;
                }
            }
            if (!found) {
                if (n >= size) {
                    size *= 2;
                    charfreqs = realloc(charfreqs, size * sizeof(charfreq_t));
                }
                charfreqs[n].c = c;
                charfreqs[n].f = 1;
                n++;
            }
        }
    }

    qsort(charfreqs, n, sizeof(charfreq_t), compar);

    char *chars = malloc(sizeof(char) * (n + 1));
    memset(chars, 0, n + 1);
    for (int i = 0; i < n; i++) {
        chars[i] = charfreqs[i].c;
    }
    free(charfreqs);
    return chars;
}

int compar(const void *a, const void *b) {
    return ((charfreq_t *)b)->f - ((charfreq_t *)a)->f;
}
