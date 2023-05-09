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
 * ECALL (it just prints a string)
 */

void e1_printf_hello_world(void)
{
  printf("Hello from enclave 1\n");
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
void debug_print_card(int **card, size_t n)
{
  printf("Card: ");
  printf("%d\n",n);
  for(int i = 0;i < n;i++){
    for (int j = 0; j < n; j++)
      printf("%d ",card[i][j]);
    printf("\n");
  } 
}

/*
 * ECALL (get plaintext card)
 */

void be_init_card(int **card, size_t n)
{
// encrypted_card[0] will be client id
// everything else is the card itself
  ocall_e1_print_string("TESTE\n");
  debug_print_card(card, n);

  return;



}
