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

  sealed_len = sgx_calc_sealed_data_size(0, data_len);
  sgx_seal_data(0, NULL, data_len, data, sealed_len, sealed_data);
  printf("sealed_len: %ld\n", sealed_len);
}



void be_get_seal_len(size_t* data_len, size_t* sealed_len) {
  *sealed_len = sgx_calc_sealed_data_size(0, *data_len);
}



/* 
//uint8_t* card_to_bytes(int **card) {
void card_to_bytes(int **card) {

// convert the int** matrix into a byte array s o m e h o w

  // first convert to char array

  // to be able to make it back into an array we're going 
  // to add some extra info as an int at the start of the array

  // so the structure will be

  // [0] - number of rows/cols (n, where the card has n² elements)
  // [1] - size in bytes of the client ID that comes after
  // [2] until 2 + whatever it is - client ID
  // after that, size of timestamp
  // then timestamp
  // then a null byte to separate the header from the card
  // then the card itself

  int client_id = card[0][0];
  int ts = card[0][1]; // timestamp

  // convert client id to char array of the algarisms
  int client_id_len = (int)((ceil(log10(client_id))+1)*sizeof(char));
  char client_id_str[client_id_len];
  sprintf(client_id_str, "%d", client_id);

  printf("client id: %s\n", client_id_str);

  return;
}
 */

/*
 * ECALL (get plaintext card)
 */

void be_init_card(uint8_t *card,size_t card_size,  sgx_sealed_data_t* sealed_data, size_t sealed_len)
{
  printf("ENCLAVE IN\n");

  debug_print_card(card, card_size);
  seal_card(card, card_size,sealed_data, sealed_len);

  printf("ENCLAVE OUT\n");
}


