/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <cstdio>
#include <math.h>

#include "BankEnclave.h"
#include "BankEnclave_t.h"  /* e1_print_string */

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char *fmt, ...)
{
  char buf[BUFSIZ] = { '\0' };
  va_list ap;
  
  va_start(ap,fmt);
  (void)vsnprintf(buf,BUFSIZ,fmt, ap);
  va_end(ap);
  ocall_e1_print_string(buf);
  return 0;
}




/*
 * ECALL (sum the elements of the array)
 */

void e1_sum_array(int *ptr,size_t n,int *sum)
{
  int s = 0;

  for(size_t i = 0;i < n;i++)
    s += ptr[i];
  *sum = s;
}

/*
 * ECALL (DEBUG print the elements of the array)
 */
void debug_print_card(uint8_t *card, size_t n)
{

  int card_size = (sizeof(card) / sizeof(card[0]));
   printf("-----card_bytes---------\n");
  for(int i = 0;i < n;i++){
    printf("%c",card[i]);
  }
  printf("\n--------------\n\n");
return;
}



// https://github.com/intel/linux-sgx/blob/master/common/inc/sgx_tseal.h
void seal_card(uint8_t* data, size_t data_len, sgx_sealed_data_t* sealed_data, size_t sealed_len) {
  printf("sealing card\n");
  printf("data_len: %ld\n", data_len);
  printf("data: %s\n", data);
  printf("sealed_len: %ld\n", sealed_len);
  printf("Sealed_data: %s\n", sealed_data);
/* 
  sgx_seal_data(
              0,          // mac len 
              NULL,       // mac
              data_len,   // card len
              data,       // card
              sealed_len, // sealed card len
              sealed_data // sealed card
              );
 */
  sgx_attributes_t attr_mask;
  attr_mask.flags = SGX_FLAGS_INITTED | SGX_FLAGS_DEBUG | SGX_FLAGS_KSS;
  attr_mask.flags &= ~SGX_FLAGS_MODE64BIT;
  attr_mask.flags &= ~(SGX_FLAGS_PROVISION_KEY | SGX_FLAGS_EINITTOKEN_KEY);
  attr_mask.xfrm = 0; 

  sgx_seal_data_ex(
    SGX_KEYPOLICY_MRENCLAVE, 
    attr_mask,   /*  Bitmask indicating which attributes the seal
                     key should be bound to. The recommendation is to set all the
                     attribute flags, except Mode 64 bit, Provision Key and Launch
                     key, and none of the XFRM attributes */
    0, 
    0,
    NULL, // TODO - CHANGE LATER
    data_len,
    data,
    sealed_len,
    sealed_data
    );


  printf("Sealed\n\n");
  printf("-------------------\n");
  printf("sealed_len: %ld\n", sealed_len);
  /* sgx_aes_gcm_data_t aes_data = sealed_data->aes_data;
  uint8_t* payload = aes_data.payload;

  
  printf("Sealed_data payload: %d\n",  payload); */

  uint8_t* tag = sealed_data->aes_data.payload;
  printf("Sealed_data tag: %d\n",  tag);
  printf("-------------------\n");
  

  printf("sealed_len: %ld\n", sealed_len);
  uint8_t* plaintext;
  uint32_t plaintext_len;

  plaintext_len = sgx_get_encrypt_txt_len(sealed_data);
  sgx_unseal_data(sealed_data, NULL, 0, plaintext, &plaintext_len);

  printf("plaintext_len: %d\n", plaintext_len);
  printf("plaintext: %s\n", plaintext);
  printf("-------------------\n");


}

/* 

int validate_response(string card, int expected, char response){
    return card[expected - 1] == response;
}

 */

void unseal_card(sgx_sealed_data_t* sealed_data, size_t sealed_size, uint8_t* plaintext, uint32_t plaintext_len) {

  printf("UNsealing card\n");
  sgx_unseal_data(sealed_data, NULL, NULL, plaintext, &plaintext_len);
}


/*
 * ECALL (get seal length)
 */
void be_get_seal_len(size_t* data_len, size_t* sealed_len) {
  *sealed_len = sgx_calc_sealed_data_size(0, *data_len);
}



/*
 * ECALL (get plaintext card)
 */


// actually a test function
void be_init_card(uint8_t *card,size_t card_size,  sgx_sealed_data_t* sealed_data, size_t sealed_len)
{
  printf("ENCLAVE IN\n");
  // debug_print_card(card, card_size);

  seal_card(card, card_size,sealed_data, sealed_len);
  printf("ENCLAVE OUT\n");
}



/*
 * ECALL (validate client request)
 */

void be_validate(sgx_sealed_data_t* sealed_card, size_t sealed_len, int x, int y, uint8_t* client_id, size_t client_id_len, int* valid) {
  // TODO actual validation, for now it's a unsealing test
  printf("ENCLAVE IN\n");
  
  uint8_t* plaintext_card;
  uint32_t plaintext_len;
  
  unseal_card(sealed_card, sealed_len, plaintext_card, plaintext_len);
  
  printf("plaintext_len: %d\n", plaintext_len);
  printf("plaintext_card: %s\n", plaintext_card);
  printf("ENCLAVE OUT\n");
}
