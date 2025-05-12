#ifndef CRYPTO_H
#define CRYPTO_H

void xor_hash(const char *input, char *output, const char *key);
void xor_encrypt(char *data, const char *key);
char *hash_password(const char *password);

#endif




