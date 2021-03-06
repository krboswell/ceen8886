#include <stdlib.h>
#include <openssl/rc4.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 8192

void print_ciphertext_in_hex(unsigned char *str);

int main(void)
{
    //define plaintext variable
    unsigned char key[BUFFER_SIZE];

    //define plaintext variable
    unsigned char plaintext[BUFFER_SIZE];

    //define plaintext variable
    unsigned char ciphertext[BUFFER_SIZE];

    //define a variable from the structure in rc4.h
    RC4_KEY rc4_key;

    strcpy(key, "The cake is a lie.");
    strcpy(plaintext, "At the end of the experiment, you will be baked.");

    // RC4 key setup
    RC4_set_key(&rc4_key, strlen(key), key);

    // RC4 encryption
    RC4(&rc4_key, strlen(plaintext), plaintext, ciphertext);

    // print out your plaintext
    printf("The plaintext is: %s\n", plaintext);

    // print out your ciphertext
    //printf("The ciphertext is: %x\n", ciphertext);
    print_ciphertext_in_hex(ciphertext);

    // RC4 key setup
    RC4_set_key(&rc4_key, strlen(key), key);
    // RC4 decryption
    RC4(&rc4_key, strlen(ciphertext), ciphertext, plaintext);

    // print out your decrypted message (plaintext)
    printf("The decrypted plaintext is: %s\n", plaintext);

    exit(1);
}

void print_ciphertext_in_hex(unsigned char *str) {
    int i, n;

    printf("The ciphertext (in hex) is:");
    n = strlen(str);
    for (i = 0; i < n; ++i) {
        if ((i % 2) == 0) {
            printf(" ");
        }
        printf("%x", (unsigned int) str[i]);
    }
    printf("\n");
}
