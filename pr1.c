#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hw1.h"

void usage(FILE *out, char *pname);

int copy_key(unsigned char *key, char *arg);
void key_schedule(unsigned char *key, unsigned int *w);

unsigned int sub_word(unsigned int w);
unsigned int rot_word(unsigned int w);

int main(int argc, char **argv) {
    int status = 0;
    unsigned char key[17];
    unsigned int w[44];

    if (argc != 3) {
        usage(stderr, argv[0]);
        status = 1;
    }

    if (status == 0) {
        status = copy_key(key, argv[1]);
    }

    if (status == 0) {
        fprintf(stdout, "Copied key: \"%s\"\n", key);
        key_schedule(key, w);
    }
}

void usage(FILE *out, char *pname) {
    fprintf(out, "Usage: %s <key> <plaintext>\n", pname);
}

int copy_key(unsigned char *key, char *arg) {
    int status;
    size_t s, i;

    status = 0;

    s = strlen(arg);
    if (s != 16) {
        fprintf(stderr, "Invalid key length: %lu\n", s);
    } else {
        memcpy(key, arg, sizeof(char) * s);
        key[s] = '\0';
    }

    return status;
}

void key_schedule(unsigned char *key, unsigned int *w) {
    int i;
    unsigned int temp;

    for (i = 0; i < 4; ++i) {
        w[i] = (key[4 * i] << 24)
            | (key[4 * i + 1] << 16)
            | (key[4 * i + 2] << 8)
            | key[4 * i + 3];
        fprintf(stdout, "w[%u]=%04x\n", i, w[i]);
    }

#if 0
    for (i = 4; i < 44; ++i) {
        temp = w[i - 1];
        if ((i % 4) == 0) {
            temp = sub_word(rot_word(temp)) ^ r_con[i/4];
        }
        w[i] = w[i - 4] ^ temp;
    }
#endif
}

unsigned int sub_word(unsigned int w) {
    unsigned int row, col;

    row = (0xff00 & w) >> 16;
    col = (0x00ff & w);

    return S_BOX[col][row];
}

unsigned int rot_word(unsigned int w) {
    unsigned char tmp, *cptr;
    int i;

    cptr = (unsigned char *) &w;
    for (i = 1; i < 4; ++i) {
    }
}
