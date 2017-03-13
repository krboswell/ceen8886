#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define P_SIZE 8
#define K_SIZE 7

typedef unsigned int value_t;

int parse_key(char *arg, value_t *key);
int parse_plaintext(char *arg, value_t *plaintext);
void print_arr(value_t *key, const char *pfx, int n);
void do_rc4(value_t *key, value_t *plaintext, value_t *ciphertext);
void swap(value_t *a, value_t *b);

int main(int argc, char **argv) {
    int status;
    value_t key[K_SIZE];
    value_t plaintext[P_SIZE];
    value_t ciphertext[P_SIZE];
    value_t plaintext_recovered[P_SIZE];

    status = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: pr2 <key> <ciphertext>\n");
        status = 1;
    }

    if (status == 0) {
        status = parse_key(argv[1], key);
    }

    if (status == 0) {
        status = parse_plaintext(argv[2], plaintext);
    }

    if (status == 0) {
        print_arr(key, "      key", K_SIZE);
        print_arr(plaintext, "plaintext", P_SIZE);
        do_rc4(key, plaintext, ciphertext);
        print_arr(ciphertext, "ciphertext", P_SIZE);
#if 0
        do_rc4(key, ciphertext, plaintext_recovered);
        print_arr(plaintext_recovered, "plaintext_recovered", P_SIZE);
#endif
    }

    return status;
}

static int parse_ascii(char *arg, value_t *tgt, int n) {
    int status;
    int i;

    status = 0;

    for (i = 0; i < K_SIZE; ++i) {
        if (arg[i] >= '0' && arg[i] <= '9') {
            tgt[i] = ((int) arg[i]) - 48;
        } else if (arg[i] >= 'A' && arg[i] <= 'F') {
            tgt[i] = ((int) arg[i]) - 55;
        } else if (arg[i] >= 'a' && arg[i] <= 'f') {
            tgt[i] = ((int) arg[i]) - 87;
        } else {
            fprintf(stderr, "Invalid character detected: %c\n", arg[i]);
            status = 1;
            break;
        }
    }

    return status;
}

int parse_key(char *arg, value_t *key) {
    return parse_ascii(arg, key, K_SIZE);
}

int parse_plaintext(char *arg, value_t *plaintext) {
    return parse_ascii(arg, plaintext, P_SIZE);
}

void print_arr(value_t *arr, const char *pfx, int n) {
    int i;

    printf("%s = [", pfx);
    for (i = 0; i < n; ++i) {
        printf("%u", arr[i]);
        if (i != (n - 1)) {
            printf(",");
        }
    }
    printf("]\n");
}

static void key_schedule(value_t *state, value_t *key) {
    value_t T[P_SIZE];
    int i, j;
    value_t tmp;

    /* Initialization */
    for (i = 0; i < P_SIZE; ++i) {
        state[i] = i;
        T[i] = key[i % K_SIZE];
    }

    /* Initial permutation of S */
    for (i = 0; i < P_SIZE; ++i) {
        j = (j + state[i] + T[i]) % P_SIZE;
        swap(&state[i], &state[j]);
    }
}

static void stream_gen(value_t *state, value_t *psga_out) {
    int i, j, index;
    value_t x, t;

    for (index = 0; index < P_SIZE; ++index) {
        i = (i + 1) % P_SIZE;
        j = (j + state[i]) % P_SIZE;
        swap(&state[i], &state[j]);
        t = (state[i] + state[j]) % P_SIZE;
        psga_out[index] = t;
    }
}

static void encrypt(value_t *plaintext, value_t *psga_out,
        value_t *ciphertext) {
    int i;

    for (i = 0; i < P_SIZE; ++i) {
        ciphertext[i] = plaintext[i] ^ psga_out[i];
    }
}

void do_rc4(value_t *key, value_t *plaintext, value_t *ciphertext) {
    value_t state[P_SIZE];
    value_t psga_out[P_SIZE];

    key_schedule(state, key);
    stream_gen(state, psga_out);
    encrypt(plaintext, psga_out, ciphertext);
}

void swap(value_t *a, value_t *b) {
    value_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}
