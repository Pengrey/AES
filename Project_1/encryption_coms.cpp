// Test implementation of Diffie-Hellman key exchange and AES-GCM encryption
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

// Generate a random 2048-bit prime number
int gen_prime(char* prime_str) {
    BIGNUM* prime = BN_new();
    BN_generate_prime_ex(prime, 2048, 1, NULL, NULL, NULL);

    // Convert the generated prime number to a string
    char* prime_hex_str = BN_bn2hex(prime);
    strncpy(prime_str, prime_hex_str, 2048); // Copy the string with length checking

    // Free the memory allocated
    OPENSSL_free(prime_hex_str);
    BN_free(prime);

    return 0;
}

int gen_rand(char* rand_str) {
    BIGNUM* rand = BN_new();
    BN_rand(rand, 2048, -1, 0);

    // Convert the generated random number to a string
    char* rand_hex_str = BN_bn2hex(rand);
    strncpy(rand_str, rand_hex_str, 2048); // Copy the string with length checking

    // Free the memory allocated
    OPENSSL_free(rand_hex_str);
    BN_free(rand);

    return 0;
}

// Compute the public key
int compute_pub_key(char* prime_str, char* rand_str, char* pub_key_str) {
    BIGNUM* prime = BN_new();
    BIGNUM* rand = BN_new();
    BIGNUM* pub_key = BN_new();
    BN_CTX* ctx = BN_CTX_new(); // Create a BN_CTX object

    // Convert the prime number and random number from strings to BIGNUMs
    BN_hex2bn(&prime, prime_str);
    BN_hex2bn(&rand, rand_str);

    // Compute the public key
    BN_mod_exp(pub_key, prime, rand, prime, ctx); // Pass the BN_CTX object as the last argument

    // Convert the public key to a string
    char* pub_key_hex_str = BN_bn2hex(pub_key);
    strncpy(pub_key_str, pub_key_hex_str, 2048); // Copy the string with length checking

    // Free the memory allocated
    OPENSSL_free(pub_key_hex_str);
    BN_free(prime);
    BN_free(rand);
    BN_free(pub_key);
    BN_CTX_free(ctx); // Free the BN_CTX object

    return 0;
}

// Compute the shared secret
int compute_shared_secret(char* pub_key_str, char* rand_str, char* prime_str, char* shared_secret_str) {
    BIGNUM* pub_key = BN_new();
    BIGNUM* rand = BN_new();
    BIGNUM* prime = BN_new();
    BIGNUM* shared_secret = BN_new();
    BN_CTX* ctx = BN_CTX_new(); // Create a BN_CTX object

    // Convert the public key, random number, and prime number from strings to BIGNUMs
    BN_hex2bn(&pub_key, pub_key_str);
    BN_hex2bn(&rand, rand_str);
    BN_hex2bn(&prime, prime_str);

    // Compute the shared secret
    BN_mod_exp(shared_secret, pub_key, rand, prime, ctx); // Pass the BN_CTX object as the last argument

    // Convert the shared secret to a string
    char* shared_secret_hex_str = BN_bn2hex(shared_secret);
    strncpy(shared_secret_str, shared_secret_hex_str, 2049); // Copy the string with length checking

    // Free the memory allocated
    OPENSSL_free(shared_secret_hex_str);
    BN_free(pub_key);
    BN_free(rand);
    BN_free(prime);
    BN_free(shared_secret);
    BN_CTX_free(ctx); // Free the BN_CTX object

    return 0;
}

// Encrypt a message using AES-GCM with the shared secret and static IV
int encrypt_msg(char* shared_secret_str, char* msg, unsigned char** ciphertext, size_t* ciphertext_len) {
    // Convert the shared secret from a string to a BIGNUM
    BIGNUM* shared_secret = BN_new();
    BN_hex2bn(&shared_secret, shared_secret_str);

    // Convert the shared secret to a byte array
    unsigned char* shared_secret_bytes = (unsigned char*)malloc(256);
    BN_bn2bin(shared_secret, shared_secret_bytes);

    // Get the length of the message
    size_t msg_len = strlen(msg);

    // Allocate memory for the ciphertext
    *ciphertext = (unsigned char*)malloc(msg_len + EVP_CIPHER_block_size(EVP_aes_256_gcm()));

    // Encrypt the message using AES-GCM
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_EncryptUpdate(ctx, *ciphertext, (int*)&msg_len, (unsigned char*)msg, (int)msg_len);
    EVP_EncryptFinal_ex(ctx, *ciphertext + msg_len, (int*)ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);

    // Free the memory allocated
    BN_free(shared_secret);
    free(shared_secret_bytes);

    return 0;
}

// Decrypt a message using AES-GCM with the shared secret and static IV
int decrypt_msg(char* shared_secret_str, unsigned char* ciphertext, size_t ciphertext_len, unsigned char** plaintext, size_t* plaintext_len) {
    // Convert the shared secret from a string to a BIGNUM
    BIGNUM* shared_secret = BN_new();
    BN_hex2bn(&shared_secret, shared_secret_str);

    // Convert the shared secret to a byte array
    unsigned char* shared_secret_bytes = (unsigned char*)malloc(256);
    BN_bn2bin(shared_secret, shared_secret_bytes);

    // Allocate memory for the plaintext
    *plaintext = (unsigned char*)malloc(ciphertext_len);

    // Decrypt the message using AES-GCM
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_DecryptUpdate(ctx, *plaintext, (int*)plaintext_len, ciphertext, (int)ciphertext_len);
    EVP_DecryptFinal_ex(ctx, *plaintext + *plaintext_len, (int*)plaintext_len);
    EVP_CIPHER_CTX_free(ctx);

    // Free the memory allocated
    BN_free(shared_secret);
    free(shared_secret_bytes);

    return 0;
}

int main(int argc, char* argv[]) {
    // Perform Diffie-Hellman key exchange
    // Alice
    char prime_str1[2049];
    char rand_str1[2049];
    char pub_key_str1[2049];

    gen_prime(prime_str1);
    gen_rand(rand_str1);
    compute_pub_key(prime_str1, rand_str1, pub_key_str1);

    // Bob
    char prime_str2[2049];
    char rand_str2[2049];
    char pub_key_str2[2049];

    gen_prime(prime_str2);
    gen_rand(rand_str2);
    compute_pub_key(prime_str2, rand_str2, pub_key_str2);

    // Alice
    char shared_secret_str1[2049];
    compute_shared_secret(pub_key_str2, rand_str1, prime_str1, shared_secret_str1);

    // Bob
    char shared_secret_str2[2049];
    compute_shared_secret(pub_key_str1, rand_str2, prime_str2, shared_secret_str2);

    // Check if the shared secrets are equal
    if (strcmp(shared_secret_str1, shared_secret_str2) == 0) {
        printf("Shared secrets are equal\n");
    } else {
        printf("Shared secrets are not equal\n");
    }

    // Encrypt and decrypt a message using AES-GCM with the shared secret
    // Alice
    unsigned char* ciphertext;
    size_t ciphertext_len;

    char msg[2049] = "This is a test message";
    encrypt_msg(shared_secret_str1, msg, &ciphertext, &ciphertext_len);

    // Bob
    unsigned char* plaintext;
    size_t plaintext_len;
    decrypt_msg(shared_secret_str2, ciphertext, ciphertext_len, &plaintext, &plaintext_len);

    // Null-terminate the plaintext array
    plaintext[plaintext_len] = '\0';

    // Print the decrypted message
    printf("Decrypted message: %s\n", (char*)plaintext);

    // Free the memory allocated
    free(ciphertext);
    free(plaintext);

    return 0;
}