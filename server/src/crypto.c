#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>

// Hashes a plain-text password using SHA-256 and converts it to a hex string
void hash_password(const char *password, char *output_hex)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Compute SHA-256 hash of the input password
    SHA256((const unsigned char *)password, strlen(password), hash);

    // Convert each byte of the hash into two hexadecimal characters
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(output_hex + (i * 2), "%02x", hash[i]);
    }

    // Null-terminate the hex string
    output_hex[64] = '\0';
}
