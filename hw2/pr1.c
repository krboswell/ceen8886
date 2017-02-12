#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hw2.h"

void usage(FILE *out, char *pname);

int copy_key(unsigned char *key, char *arg);
void generate_round_keys(unsigned char *key, unsigned int *w);
void print_round_keys(unsigned int *w);

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
        generate_round_keys(key, w);
        print_round_keys(w);
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

void generate_round_keys(unsigned char *key, unsigned int *w) {
    int i;
    unsigned int temp, gw, rcon;

    for (i = 0; i < 4; ++i) {
        w[i] = (key[4 * i] << 24)
            | (key[4 * i + 1] << 16)
            | (key[4 * i + 2] << 8)
            | key[4 * i + 3];
    }

    for (i = 4; i < 44; ++i) {
        temp = w[i - 1];
        if ((i % 4) == 0) {
            gw = sub_word(rot_word(temp));
            rcon = RCON[i/4] << 24;
            temp = gw ^ rcon;
        }
        w[i] = w[i - 4] ^ temp;
    }
}

void print_round_keys(unsigned int *w) {
    int i, j, round, index;

    round = 0;
    index = 0;
    for (i = 0; i < 11; ++i) {
        fprintf(stdout, "round = %-2d: ", round);
        for (j = 0; j < 4; ++j) {
            fprintf(stdout, "%08x ", w[index]);
            ++index;
        }
        fprintf(stdout, "\n");
        ++round;
    }
}

unsigned int sub_word(unsigned int w) {
    unsigned int retval;
    unsigned int row, col;
    unsigned char *cptr;
    int i;

    retval = w;
    cptr = (unsigned char *) &retval;

    for (i = 0; i < 4; ++i) { 
        row = (0xf0 & cptr[i]) >> 4;
        col = (0x0f & cptr[i]);
        cptr[i] = S_BOX[row][col];
    }

    return retval;
}

unsigned int rot_word(unsigned int w) {
    unsigned retval;
    unsigned char tmp, *cptr;
    int i;

    retval = w;

    cptr = (unsigned char *) &retval;
    i = 3;

    tmp = cptr[3];
    for (i = 3; i > 0; --i) {
        cptr[i] = cptr[i - 1];
    };
    cptr[0] = tmp;

    return retval;
}
