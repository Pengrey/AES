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
#include <random>

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "BankEnclave.h"
#include "BankEnclave_t.h"  /* e1_print_string */
#define SIZE_CARD 8

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

// https://github.com/intel/linux-sgx/blob/master/common/inc/sgx_tseal.h
void seal_card(uint8_t* data, size_t data_len, sgx_sealed_data_t* sealed_data, size_t sealed_len) {
  printf("sealing card\n");
  printf("data_len: %ld\n", data_len);
  printf("data: %s\n", data);
  printf("sealed_len: %ld\n", sealed_len);
  printf("Sealed_data: %s\n", sealed_data); */
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
/*  
  sgx_attributes_t attr_mask;
  attr_mask.flags = SGX_FLAGS_INITTED | SGX_FLAGS_DEBUG | SGX_FLAGS_KSS;
  attr_mask.flags &= ~SGX_FLAGS_MODE64BIT;
  attr_mask.flags &= ~(SGX_FLAGS_PROVISION_KEY | SGX_FLAGS_EINITTOKEN_KEY);
  attr_mask.xfrm = 0; 
 *//* 
  sgx_seal_data_ex(
    SGX_KEYPOLICY_MRENCLAVE, 
    attr_mask,  */  /*  Bitmask indicating which attributes the seal
                     key should be bound to. The recommendation is to set all the
                     attribute flags, except Mode 64 bit, Provision Key and Launch
                     key, and none of the XFRM attributes */
  /*   0, 
    0,
    NULL, // TODO - CHANGE LATER
    data_len,
    data,
    sealed_len,
    sealed_data
    ); */
/* 

  printf("Sealed\n\n");
  printf("-------------------\n");
  printf("sealed_len: %ld\n", sealed_len); */
  /* sgx_aes_gcm_data_t aes_data = sealed_data->aes_data;
  uint8_t* payload = aes_data.payload;

  
  printf("Sealed_data payload: %d\n",  payload); */
/* 
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


} */

/* 

int validate_response(string card, int expected, char response){
    return card[expected - 1] == response;
}

 */

uint8_t* unseal_card(uint8_t *sealed_card, size_t sealed_size) {
  //uint32_t mac_text_len = sgx_get_add_mac_txt_len((const sgx_sealed_data_t *)sealed_blob);
  uint32_t mac_text_len=0;

  uint32_t plaintext_len = sgx_get_encrypt_txt_len((const sgx_sealed_data_t *)sealed_card);
  printf("plaintext_len: %d\n", plaintext_len);

  uint8_t *plaintext_card = (uint8_t *)malloc(plaintext_len);


  sgx_status_t ret = sgx_unseal_data((const sgx_sealed_data_t *)sealed_card, NULL, 0, plaintext_card, &plaintext_len);
  if (ret != SGX_SUCCESS)
  {
    printf("Unseal failed\n");
    free(plaintext_card);
    return (uint8_t *)"";
  }

  return plaintext_card;
}



/*
 * ECALL (get seal length)
 */
void be_get_seal_len(size_t* data_len, size_t* sealed_len) {
  *sealed_len = sgx_calc_sealed_data_size(0, *data_len);
}


void append_to_card_log(uint8_t** card, size_t* card_size, char* message,int message_size){
  int ts;
  ocall_be_get_ts(&ts);
  printf("AAAA\n");

  std::string log_message = (std::to_string(ts) + std::string(" -- ")+std::string(message)+"\n") ;
  int log_message_size = log_message.size();
  printf("AAAA\n");
  printf("log_message_size: %d\n", log_message_size);
  printf("log_message: %s\n", log_message.c_str());

  printf("BEOFRE card_size: %d\n", *card_size);

 //print card
  printf("card: %s\n", *card);

  int new_card_size = *card_size + log_message_size;
  printf("AAAA\n");
  printf("new_card_size: %d\n", new_card_size);
  uint8_t* new_card = (uint8_t *)malloc(new_card_size*sizeof(uint8_t));
  printf("AAAA\n");
  int better_card_size = *card_size;


  memcpy(new_card, *card, better_card_size);
  printf("AAAA\n");
  memcpy(new_card+better_card_size, (uint8_t*)log_message.c_str(), log_message_size);
  printf("AAAA\n");

  *card_size = new_card_size;
  printf("AFTER card_size: %d\n", *card_size);

  for(int i=0; i<new_card_size; i++){
    printf("%c", new_card[i]);
  }

  *card = new_card;
}

/*
 * ECALL (get plaintext card)
 */

// First Card Seal
// Plaintext in, Sealed out
// Created Log
void be_init_card(uint8_t *card,size_t card_size,uint8_t* sealed_card, 
                  size_t sealed_card_len, size_t* updated_len)
{
  int ts;
  ocall_be_get_ts(&ts);
  
 
  append_to_card_log(&card, &card_size, "Card created.", 12);
  printf("after log card_size: %d\n", card_size);
  sealed_card_len = sgx_calc_sealed_data_size(0, card_size); // CHANGE FOR ADDING MAC TEXT
  if (sealed_card_len == UINT32_MAX){
    printf("error unexpected\n");
   return;
  }
  // change card len to new one inc log
  memcpy(updated_len, &sealed_card_len, sizeof(sealed_card_len)); 

  uint8_t *temp_sealed_buf = (uint8_t *)malloc(sealed_card_len);
  if(temp_sealed_buf == NULL){
    printf("out of memory\n");  
    return;
  }

  sgx_status_t  err = sgx_seal_data(0 , NULL, card_size, card, sealed_card_len, 
                      (sgx_sealed_data_t *)temp_sealed_buf);
  if (err == SGX_SUCCESS)
  {  
    // Copy the sealed data to outside buffer
    memcpy(sealed_card, temp_sealed_buf, sealed_card_len); //... DO **NOT** ADD THE & (again) (it's NOT on the first arg) STOP ÃDDING IT
  } 
  return;
}



int get_pos(){
   // Generate a secure random number between 0 and card_size
    std::random_device rd;
    std::default_random_engine engine(rd());

    // Define the range of the random number
    int min = 0;
    int max = SIZE_CARD * SIZE_CARD * 3;

    // Generate the random number between min and max
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine);
}

/*
 * ECALL (validate client request)
 */

void be_validate( uint8_t* sealed_card, size_t sealed_size, uint8_t* client_id, size_t client_id_len, int* valid, uint8_t* sealed_card_out, size_t sealed_card_out_len, size_t* updated_len) {

  int pos = get_pos();
  int* position = &pos;
  char response = '0';
  char* response_p = &response;

  char res = ocall_be_get_response(response_p, pos);
  sgx_sealed_data_t* test=(sgx_sealed_data_t *)sealed_card;
  printf("sealed size: %d\n",sealed_size);
    printf("-----sealed buf---------\n");
  for(int i=0;i<sealed_size/8;i++){
    printf("%02x",test[i]);
  }
  printf("\n--------------\n\n"); 
  uint32_t card_size = sgx_get_encrypt_txt_len((sgx_sealed_data_t *)sealed_card);
  printf("E: card_size: %d\n", card_size);

  uint8_t* plaintext_card = unseal_card(sealed_card, sealed_size);
  
  printf("E: plaintext_card: %s\n", plaintext_card);
  
  
  append_to_card_log(&plaintext_card, (size_t*) &card_size, "Card Validation Requested.", sizeof("Card Validation Requested."));
 
  std::string message = "Expected Value: " + std::to_string(*position)
                      + " Received Value: " + std::to_string(response);
  append_to_card_log(&plaintext_card, (size_t*)&card_size, (char*)message.c_str(), message.size());
  
 for(int i=0;i<card_size;i++){
    printf("%c",plaintext_card[i]);
  }

  printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  int result = (*(plaintext_card+*position) == response);
  printf("E: result: %d\n", result);
  if(result){
      append_to_card_log(&plaintext_card, (size_t*)&card_size, "Success!", sizeof("Success!"));

  } else {
      append_to_card_log(&plaintext_card, (size_t*)&card_size, "Failed...", sizeof("Failed..."));

  }

  
  size_t* sealed_card_len;
  *sealed_card_len = sgx_calc_sealed_data_size(0, card_size); // CHANGE FOR ADDING MAC TEXT
  if (*sealed_card_len == UINT32_MAX){
    printf("error unexpected\n");
   return;
  }

  // change card len to new one inc log
  memcpy(updated_len, &sealed_card_len, sizeof(*sealed_card_len)); 

  uint8_t *temp_sealed_buf = (uint8_t *)malloc(*sealed_card_len);
  if(temp_sealed_buf == NULL){
    printf("out of memory\n");  
    return;
  }

  sgx_status_t  err = sgx_seal_data(0 , NULL, card_size, plaintext_card, *sealed_card_len, 
                      (sgx_sealed_data_t *)temp_sealed_buf);
  if (err == SGX_SUCCESS)
  {  
    // Copy the sealed data to outside buffer
    memcpy(sealed_card_out, temp_sealed_buf, *sealed_card_len); //... DO **NOT** ADD THE & (again) (it's NOT on the first arg) STOP ÃDDING IT
  } 
  return;
}
