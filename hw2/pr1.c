#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "hw2.h"

void usage(FILE *out, char *pname);

int copy_key(unsigned char *key, char *orig, int size);
void generate_round_keys(unsigned char *key, unsigned int *w);
void print_round_keys(unsigned int *w);

unsigned int sub_word(unsigned int w);
void sub_bytes(unsigned int *state);
unsigned int sub_byte(unsigned char byte);
unsigned int rot_word(unsigned int w);
void shift_rows(unsigned int *state);
void mix_columns(unsigned int *state);

void add_round_key(unsigned int *state, unsigned int *w);
void print_state(unsigned int *state);

int main(int argc, char **argv) {
    int status;
    unsigned char key[33];
    unsigned int w[44];
    unsigned int state[4];

    status = 0;

    if (argc != 3) {
        usage(stderr, argv[0]);
        status = 1;
    }

    if (status == 0) {
        status = copy_key(key, argv[1], 32);
    }

    if (status == 0) {
        status = copy_key((unsigned char *) state, argv[2], 32);
    }

    if (status == 0) {
        status = hex2ascii((unsigned char *) state);
    }

    if (status == 0) {
        status = hex2ascii(key);
    }

    if (status == 0) {
        generate_round_keys(key, w);

        printf("Round keys:\n");
        print_round_keys(w);
        printf("\n");

        printf("Initial State:\n");
        print_state(state);
        printf("\n");

        add_round_key(state, &w[0]);
        printf("State after AddRoundKey:\n");
        print_state(state);
        printf("\n");

        sub_bytes(state);
        printf("State after SubBytes:\n");
        print_state(state);
        printf("\n");

        shift_rows(state);
        printf("State after ShiftRows:\n");
        print_state(state);
        printf("\n");

        mix_columns(state);
        printf("State after MixColumns:\n");
        print_state(state);
        printf("\n");
    }
}

void usage(FILE *out, char *pname) {
    fprintf(out, "Usage: %s <key> <plaintext>\n", pname);
}

int copy_key(unsigned char *key, char *orig, int size) {
    int status;
    size_t s, i;

    status = 0;

    memcpy(key, orig, sizeof(unsigned char) * size);

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
            fprintf(stdout, "%02x ", (w[index] & 0xff000000) >> 24);
            fprintf(stdout, "%02x ", (w[index] & 0x00ff0000) >> 16);
            fprintf(stdout, "%02x ", (w[index] & 0x0000ff00) >> 8);
            fprintf(stdout, "%02x ", (w[index] & 0x000000ff));

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

void sub_bytes(unsigned int *state) {
    int i;
    unsigned char *bytes;

    bytes = (unsigned char *) state;

    for (i = 0; i < 16; ++i) {
        bytes[i] = sub_byte(bytes[i]);
    }
}

unsigned int sub_byte(unsigned char byte) {
    unsigned int row, col;
    row = (0xf0 & byte) >> 4;
    col = (0x0f & byte);
    return S_BOX[row][col];
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

void shift_rows(unsigned int *state) {
    unsigned int tmp, tmp2, tmp3;
    unsigned char *bytes;

    bytes = (unsigned char *) state;
    tmp = bytes[1];
    bytes[1] = bytes[5];
    bytes[5] = bytes[9];
    bytes[9] = bytes[13];
    bytes[13] = tmp;

    tmp = bytes[2];
    tmp2 = bytes[6];
    bytes[2] = bytes[10];
    bytes[6] = bytes[14];
    bytes[10] = tmp;
    bytes[14] = tmp2;

    tmp = bytes[3];
    tmp2 = bytes[7];
    tmp3 = bytes[11];
    bytes[3] = bytes[15];
    bytes[7] = tmp;
    bytes[11] = tmp2;
    bytes[15] = tmp3;
}

static unsigned char gmul(unsigned char a, unsigned char b) {
    unsigned char p;
    unsigned char i;
    unsigned char hi_bit_set;

    p = 0;

    for(i = 0; i < 8; i++) {
        if((b & 1) == 1) {
            p ^= a;
        }
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if(hi_bit_set == 0x80) {
            a ^= 0x1b;
        }
        b >>= 1;
    }

    return p;
}

static void gmix_column(unsigned char *r) {
    unsigned char a[4];
    unsigned char i;
    for(i=0; i<4; ++i) {
        a[i] = r[i];
    }
    r[0] = gmul(a[0], 2) ^ gmul(a[3], 1) ^ gmul(a[2], 1) ^ gmul(a[1], 3);
    r[1] = gmul(a[1], 2) ^ gmul(a[0], 1) ^ gmul(a[3], 1) ^ gmul(a[2], 3);
    r[2] = gmul(a[2], 2) ^ gmul(a[1], 1) ^ gmul(a[0], 1) ^ gmul(a[3], 3);
    r[3] = gmul(a[3], 2) ^ gmul(a[2], 1) ^ gmul(a[1], 1) ^ gmul(a[0], 3);
}

void mix_columns(unsigned int *state) {
    int i;

    for (i = 0; i < 4; ++i) {
        gmix_column((unsigned char *) (state + i));
    }
}

void add_round_key(unsigned int *state, unsigned int *w) {
    int i, j;
    unsigned char *state_row;
    unsigned char w_val;

    static unsigned int mask[4] = {
        0xff000000,
        0x00ff0000,
        0x0000ff00,
        0x000000ff
    };

    static unsigned int shift[4] = {
        24,
        16,
        8,
        0
    };

    for (i = 0; i < 4; ++i) {
        state_row = (unsigned char *) &state[i];
        for (j = 0; j < 4; ++j) {
            w_val = (unsigned char)((w[i] & mask[j]) >> shift[j]);
            state_row[j] = state_row[j] ^ w_val;
        }
    }
}

void print_state(unsigned int *state) {
    int i, j;
    unsigned char *bytes;

    bytes = (unsigned char *) state;

    fprintf(stdout, "%02x ", (unsigned int) bytes[0]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[4]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[8]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[12]);
    fprintf(stdout, "\n");
    fprintf(stdout, "%02x ", (unsigned int) bytes[1]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[5]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[9]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[13]);
    fprintf(stdout, "\n");
    fprintf(stdout, "%02x ", (unsigned int) bytes[2]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[6]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[10]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[14]);
    fprintf(stdout, "\n");
    fprintf(stdout, "%02x ", (unsigned int) bytes[3]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[7]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[11]);
    fprintf(stdout, "%02x ", (unsigned int) bytes[15]);
    fprintf(stdout, "\n");
}

