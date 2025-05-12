#include "crypto.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void xor_hash(const char *input, char *output, const char *key) {
    size_t len = strlen(input);
    size_t keylen = strlen(key);
    for (size_t i = 0; i < len; i++) {
        sprintf(output + (i * 2), "%02x", (unsigned char)(input[i] ^ key[i % keylen]));
    }
    output[len * 2] = '\0';
}


void xor_encrypt(char *data, const char *key) {
    size_t len = strlen(data);
    size_t klen = strlen(key);
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key[i % klen];
    }
}

char *hash_password(const char *password) {
    char *hash = strdup(password); // À remplacer par un vrai hash pour production
    xor_encrypt(hash, "secret");
    return hash;
}



