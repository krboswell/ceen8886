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

void add_round_key(unsigned int *state, unsigned int *w);
void print_state(unsigned int *state);

int main(int argc, char **argv) {
    int status;
    char c;
    int hex_option;
    int argidx;
    int nargs;
    int done;
    unsigned char key[33];
    unsigned int w[44];
    unsigned int state[4];

    status = 0;

    hex_option = 0;
    opterr = 0;

    done = 0;
    while (!done && (c = getopt(argc, argv, "hx:")) != 1) {
        switch (c) {
            case 'h':
                status = opterr;
                usage(stdout, argv[0]);
                break;
            case 'x':
                printf("hex option selected\n");
                hex_option = 1;
                break;
            case '?':
                if (optopt == 'c') {
                    fprintf(stderr, "Option .%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option '.%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                }
                return 1;
            default:
                done = 1;
                break;
        }
    }

    fprintf(stdout, "status=%d, optind=%d, argc=%d\n", status, optind, argc);
    if (optind > 1) {
        nargs = argc - optind + 1;
        argidx = optind - 1;
    } else {
        nargs = argc - optind;
        argidx = optind;
    }
    if (status == 0 && nargs != 2) {
        usage(stderr, argv[0]);
        status = 1;
    }

    if (status == 0) {
        if (hex_option) {
            status = copy_key(key, argv[argidx], 32);
        } else {
            status = copy_key(key, argv[argidx], 16);
        }
    }

    if (status == 0) {
        status = copy_key((unsigned char *) state, argv[argidx + 1], 32);
    }

    if (status == 0) {
        status = hex2ascii((unsigned char *) state);
    }

    if (status == 0 && hex_option) {
        status = hex2ascii(key);
    }

    if (status == 0) {
        generate_round_keys(key, w);

        printf("Round keys:\n");
        print_round_keys(w);
        printf("\n");

        printf("State 0:\n");
        print_state(state);
        printf("\n");

        add_round_key(state, &w[0]);
        printf("State 1:\n");
        print_state(state);
        printf("\n");

        sub_bytes(state);
        printf("State 1:\n");
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

    s = strlen(orig);
    if (s != size) {
        fprintf(stderr, "Invalid string length: %lu\n", s);
    } else {
        memcpy(key, orig, sizeof(char) * s);
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

