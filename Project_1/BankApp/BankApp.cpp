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


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <utility>

#include <random>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <string>
using namespace std;

#include "sgx_utils.h"
#include "sgx_urts.h"
#include "BankApp.h"
#include "BankEnclave_u.h"


#define SIZE_CARD 8
/*
 * Error reporting
 */

typedef struct _sgx_errlist_t
{
  sgx_status_t error_number;
  const char *message;
}
sgx_errlist_t;

static sgx_errlist_t sgx_errlist[] =
{ /* error list extracted from /opt/intel/sgxsdk/include/sgx_error.h */
  { SGX_SUCCESS                                  ,"All is well!"                                                                                                                                  },
  { SGX_ERROR_UNEXPECTED                         ,"Unexpected error"                                                                                                                              },
  { SGX_ERROR_INVALID_PARAMETER                  ,"The parameter is incorrect"                                                                                                                    },
  { SGX_ERROR_OUT_OF_MEMORY                      ,"Not enough memory is available to complete this operation"                                                                                     },
  { SGX_ERROR_ENCLAVE_LOST                       ,"Enclave lost after power transition or used in child process created by linux:fork()"                                                          },
  { SGX_ERROR_INVALID_STATE                      ,"SGX API is invoked in incorrect order or state"                                                                                                },
  { SGX_ERROR_FEATURE_NOT_SUPPORTED              ,"Feature is not supported on this platform"                                                                                                     },
  { SGX_PTHREAD_EXIT                             ,"Enclave is exited with pthread_exit()"                                                                                                         },
  { SGX_ERROR_MEMORY_MAP_FAILURE                 ,"Failed to reserve memory for the enclave"                                                                                                      },
  { SGX_ERROR_INVALID_FUNCTION                   ,"The ecall/ocall index is invalid"                                                                                                              },
  { SGX_ERROR_OUT_OF_TCS                         ,"The enclave is out of TCS"                                                                                                                     },
  { SGX_ERROR_ENCLAVE_CRASHED                    ,"The enclave is crashed"                                                                                                                        },
  { SGX_ERROR_ECALL_NOT_ALLOWED                  ,"The ECALL is not allowed at this time, e.g. ecall is blocked by the dynamic entry table, or nested ecall is not allowed during initialization" },
  { SGX_ERROR_OCALL_NOT_ALLOWED                  ,"The OCALL is not allowed at this time, e.g. ocall is not allowed during exception handling"                                                    },
  { SGX_ERROR_STACK_OVERRUN                      ,"The enclave is running out of stack"                                                                                                           },
  { SGX_ERROR_UNDEFINED_SYMBOL                   ,"The enclave image has undefined symbol"                                                                                                        },
  { SGX_ERROR_INVALID_ENCLAVE                    ,"The enclave image is not correct"                                                                                                              },
  { SGX_ERROR_INVALID_ENCLAVE_ID                 ,"The enclave id is invalid"                                                                                                                     },
  { SGX_ERROR_INVALID_SIGNATURE                  ,"The signature is invalid"                                                                                                                      },
  { SGX_ERROR_NDEBUG_ENCLAVE                     ,"The enclave is signed as product enclave, and can not be created as debuggable enclave"                                                        },
  { SGX_ERROR_OUT_OF_EPC                         ,"Not enough EPC is available to load the enclave"                                                                                               },
  { SGX_ERROR_NO_DEVICE                          ,"Can't open SGX device"                                                                                                                         },
  { SGX_ERROR_MEMORY_MAP_CONFLICT                ,"Page mapping failed in driver"                                                                                                                 },
  { SGX_ERROR_INVALID_METADATA                   ,"The metadata is incorrect"                                                                                                                     },
  { SGX_ERROR_DEVICE_BUSY                        ,"Device is busy, mostly EINIT failed"                                                                                                           },
  { SGX_ERROR_INVALID_VERSION                    ,"Metadata version is inconsistent between uRTS and sgx_sign or uRTS is incompatible with current platform"                                      },
  { SGX_ERROR_MODE_INCOMPATIBLE                  ,"The target enclave 32/64 bit mode or sim/hw mode is incompatible with the mode of current uRTS"                                                },
  { SGX_ERROR_ENCLAVE_FILE_ACCESS                ,"Can't open enclave file"                                                                                                                       },
  { SGX_ERROR_INVALID_MISC                       ,"The MiscSelct/MiscMask settings are not correct"                                                                                               },
  { SGX_ERROR_INVALID_LAUNCH_TOKEN               ,"The launch token is not correct"                                                                                                               },
  { SGX_ERROR_MAC_MISMATCH                       ,"Indicates verification error for reports, sealed datas, etc"                                                                                   },
  { SGX_ERROR_INVALID_ATTRIBUTE                  ,"The enclave is not authorized, e.g., requesting invalid attribute or launch key access on legacy SGX platform without FLC"                     },
  { SGX_ERROR_INVALID_CPUSVN                     ,"The cpu svn is beyond platform's cpu svn value"                                                                                                },
  { SGX_ERROR_INVALID_ISVSVN                     ,"The isv svn is greater than the enclave's isv svn"                                                                                             },
  { SGX_ERROR_INVALID_KEYNAME                    ,"The key name is an unsupported value"                                                                                                          },
  { SGX_ERROR_SERVICE_UNAVAILABLE                ,"Indicates aesm didn't respond or the requested service is not supported"                                                                       },
  { SGX_ERROR_SERVICE_TIMEOUT                    ,"The request to aesm timed out"                                                                                                                 },
  { SGX_ERROR_AE_INVALID_EPIDBLOB                ,"Indicates epid blob verification error"                                                                                                        },
  { SGX_ERROR_SERVICE_INVALID_PRIVILEGE          ," Enclave not authorized to run, .e.g. provisioning enclave hosted in an app without access rights to /dev/sgx_provision"                       },
  { SGX_ERROR_EPID_MEMBER_REVOKED                ,"The EPID group membership is revoked"                                                                                                          },
  { SGX_ERROR_UPDATE_NEEDED                      ,"SGX needs to be updated"                                                                                                                       },
  { SGX_ERROR_NETWORK_FAILURE                    ,"Network connecting or proxy setting issue is encountered"                                                                                      },
  { SGX_ERROR_AE_SESSION_INVALID                 ,"Session is invalid or ended by server"                                                                                                         },
  { SGX_ERROR_BUSY                               ,"The requested service is temporarily not available"                                                                                            },
  { SGX_ERROR_MC_NOT_FOUND                       ,"The Monotonic Counter doesn't exist or has been invalided"                                                                                     },
  { SGX_ERROR_MC_NO_ACCESS_RIGHT                 ,"Caller doesn't have the access right to specified VMC"                                                                                         },
  { SGX_ERROR_MC_USED_UP                         ,"Monotonic counters are used out"                                                                                                               },
  { SGX_ERROR_MC_OVER_QUOTA                      ,"Monotonic counters exceeds quota limitation"                                                                                                   },
  { SGX_ERROR_KDF_MISMATCH                       ,"Key derivation function doesn't match during key exchange"                                                                                     },
  { SGX_ERROR_UNRECOGNIZED_PLATFORM              ,"EPID Provisioning failed due to platform not recognized by backend server"                                                                     },
  { SGX_ERROR_UNSUPPORTED_CONFIG                 ,"The config for trigging EPID Provisiong or PSE Provisiong&LTP is invalid"                                                                      },
  { SGX_ERROR_NO_PRIVILEGE                       ,"Not enough privilege to perform the operation"                                                                                                 },
  { SGX_ERROR_PCL_ENCRYPTED                      ,"trying to encrypt an already encrypted enclave"                                                                                                },
  { SGX_ERROR_PCL_NOT_ENCRYPTED                  ,"trying to load a plain enclave using sgx_create_encrypted_enclave"                                                                             },
  { SGX_ERROR_PCL_MAC_MISMATCH                   ,"section mac result does not match build time mac"                                                                                              },
  { SGX_ERROR_PCL_SHA_MISMATCH                   ,"Unsealed key MAC does not match MAC of key hardcoded in enclave binary"                                                                        },
  { SGX_ERROR_PCL_GUID_MISMATCH                  ,"GUID in sealed blob does not match GUID hardcoded in enclave binary"                                                                           },
  { SGX_ERROR_FILE_BAD_STATUS                    ,"The file is in bad status, run sgx_clearerr to try and fix it"                                                                                 },
  { SGX_ERROR_FILE_NO_KEY_ID                     ,"The Key ID field is all zeros, can't re-generate the encryption key"                                                                           },
  { SGX_ERROR_FILE_NAME_MISMATCH                 ,"The current file name is different then the original file name (not allowed, substitution attack)"                                             },
  { SGX_ERROR_FILE_NOT_SGX_FILE                  ,"The file is not an SGX file"                                                                                                                   },
  { SGX_ERROR_FILE_CANT_OPEN_RECOVERY_FILE       ,"A recovery file can't be opened, so flush operation can't continue (only used when no EXXX is returned)"                                       },
  { SGX_ERROR_FILE_CANT_WRITE_RECOVERY_FILE      ,"A recovery file can't be written, so flush operation can't continue (only used when no EXXX is returned)"                                      },
  { SGX_ERROR_FILE_RECOVERY_NEEDED               ,"When openeing the file, recovery is needed, but the recovery process failed"                                                                   },
  { SGX_ERROR_FILE_FLUSH_FAILED                  ,"fflush operation (to disk) failed (only used when no EXXX is returned)"                                                                        },
  { SGX_ERROR_FILE_CLOSE_FAILED                  ,"fclose operation (to disk) failed (only used when no EXXX is returned)"                                                                        },
  { SGX_ERROR_UNSUPPORTED_ATT_KEY_ID             ,"platform quoting infrastructure does not support the key"                                                                                      },
  { SGX_ERROR_ATT_KEY_CERTIFICATION_FAILURE      ,"Failed to generate and certify the attestation key"                                                                                            },
  { SGX_ERROR_ATT_KEY_UNINITIALIZED              ,"The platform quoting infrastructure does not have the attestation key available to generate quote"                                             },
  { SGX_ERROR_INVALID_ATT_KEY_CERT_DATA          ,"TThe data returned by the platform library's sgx_get_quote_config() is invalid"                                                                },
  { SGX_ERROR_PLATFORM_CERT_UNAVAILABLE          ,"The PCK Cert for the platform is not available"                                                                                                },
  { SGX_INTERNAL_ERROR_ENCLAVE_CREATE_INTERRUPTED,"The ioctl for enclave_create unexpectedly failed with EINTR"                                                                                   }
};



void print_error_message(sgx_status_t ret,const char *sgx_function_name)
{
  size_t ttl = sizeof(sgx_errlist) / sizeof(sgx_errlist[0]);
  size_t idx;

  if(sgx_function_name != NULL)
    printf("Function: %s\n",sgx_function_name);
  for(idx = 0;idx < ttl;idx++)
  {
    if(ret == sgx_errlist[idx].error_number)
    {
      printf("Error: %s\n",sgx_errlist[idx].message);
      break;
    }
  }
  if(idx == ttl)
    printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n",ret);
}


/*
 * BankEnclave stuff
 */

sgx_enclave_id_t global_eid1 = 0;

int initialize_bankenclave(void)
{
  sgx_status_t ret;
    
  if((ret = sgx_create_enclave(BANKENCLAVE_FILENAME,SGX_DEBUG_FLAG,NULL,NULL,&global_eid1,NULL)) != SGX_SUCCESS)
  {
    print_error_message(ret,"sgx_create_enclave");
    return -1;
  }
  return 0;
}

void ocall_e1_print_string(const char *str)
{
  printf("%s",str);
}

char ocall_be_get_response(int position)
{
  //printf("App: position %d\n",position);
  int row = position / (SIZE_CARD * 3);

 string row_label = "";
  while (row >= 0) {
      row_label = (char)(row % 26 + 65) + row_label;
      row /= 26;
      row--;
  }

  int col = (position % (SIZE_CARD * 3)) / 3;
  int letter = (position % (SIZE_CARD * 3)) % 3;

  // Ask the user to write the number for the given position
  cout << "Write the number for the position " << row_label << "-" << col + 1 << "-" << letter + 1 << ": ";
  char number;
  cin >> number; // will only get one char, as intended!

  return number; 
}


int ocall_be_get_ts()
{
  return time(NULL);
}

int generate_card(string client_id)                
{    std::string filename = client_id + ".txt";

    ofstream fout(filename);              // The numbers are stored in a file named "card.txt"
    srand(time(NULL));

    // first row
    fout << "     ";                          
    for (int k = 1; k <= SIZE_CARD; k++) {
        if (k < 100)                        // The number is padded with zeros to make it 3 digits
            fout << "0";
        if (k < 10) 
            fout << "0";

        fout << k << " ";                   // The numbers are separated by a space
    }
    fout << endl;                           // The numbers are separated by a new line

    for (int i = 0; i < SIZE_CARD; i++)     // The program also prints the numbers on the screen
    { 
        if (i < 26) {                       // The first column is labeled with letters from A to Z
            fout << "  " << (char)(i + 65) << "  ";
        } else if (i < 702) {
            fout << " " << (char)(i / 26 + 64) << (char)(i % 26 + 65) << "  ";
        } else {
            fout << (char)(i / 676 + 64) << (char)(i / 26 % 26 + 65) << (char)(i % 26 + 65) << "  ";
        }

        for (int j = 0; j < SIZE_CARD; j++)
        {
            int num = rand() % 999 + 1;     // The range of the numbers are from the range 001 to 999

            if (num < 100)                  // The number is padded with zeros to make it 3 digits
                fout << "0";
            if (num < 10) 
                fout << "0";

            fout << num << " ";             // The numbers are separated by a space
        }

        fout << endl;                       // The numbers are separated by a new line
    }
    fout.close();
    return 0;
}

string parse_card(string client_id) {
    std::string filename = client_id + ".txt";

    ifstream fin(filename);
    // Ignore first line
    string line;
    getline(fin, line);

    // Read the rest of the file and ignore first 3 characters of each line
    string result;
    while (getline(fin, line)) {
        // Remove the first 3 characters
        line.erase(0, 3);
        // Remove white spaces from the line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        // Add the line to the result string
        result += line;
    }
    fin.close();
    return result+"\n"+client_id+"\n";
}


string init_card(string client_id){
  generate_card(client_id); 
  return parse_card(client_id);
}


void get_card_logs(string client_id){
  
  FILE *file_ptr;
  char filename[client_id.length()+5];
  strcpy(filename,client_id.c_str());
  strcat(filename,".bin");

  file_ptr = fopen(filename,"rb");  
  fseek(file_ptr, 0, SEEK_END);
  long file_size = ftell(file_ptr);
  fseek(file_ptr, 0, SEEK_SET);
  uint8_t* temp_buf = (uint8_t*) calloc(file_size,sizeof(uint8_t));
  fread(temp_buf, 1, file_size, file_ptr);
  fclose(file_ptr);

  sgx_status_t ret;

  uint8_t* client_id_bytes = (uint8_t*) calloc(client_id.length(),sizeof(uint8_t));
  if((ret = be_get_logs(global_eid1,temp_buf, file_size, client_id_bytes,(size_t)client_id.length())) != SGX_SUCCESS)
  {
    print_error_message(ret,"be_get_logs");
    return;
  }  
}


// Creates a card for the client, sends it to the enclave to be sealed
// and saves it to .bin file
int init_client(std::string client_id){

  std::string card = init_card(client_id);
// print file to cout

    printf("\nYour card:\n");
    std::ifstream f(client_id+".txt");

    if (f.is_open())
        std::cout << f.rdbuf();
  
    printf("Save it somewhere safe!\n\n");
  //debug
  /* 
  printf("-----card---------\n");
  cout << card << endl;
  printf("--------------\n\n"); 
 */

 
  // turn card into uint8_t array
  uint8_t* card_bytes = (uint8_t*) malloc(sizeof(uint8_t) * card.length());
  memcpy(card_bytes,card.c_str(),card.length());


  // cout << "card_size: " << card.length() << endl;


  // send card to enclave
  sgx_status_t ret;
  sgx_sealed_data_t* sealed_card ;
  //uint32_t sealed_size = sgx_calc_sealed_data_size(0,card.length());
  size_t sealed_len ;
  size_t* sealed_len_p = &sealed_len;
  size_t card_size = card.length();
  size_t* card_size_p = &card_size;


  if((ret = be_get_seal_len(global_eid1,card_size_p, sealed_len_p,client_id.length())) != SGX_SUCCESS)
  {
    print_error_message(ret,"be_get_seal_len");
    return 1;
  }  
  if (ret != SGX_SUCCESS)
    {
        sgx_destroy_enclave(global_eid1);

        return false;
    }
    else if(sealed_len == UINT32_MAX)
    {
        sgx_destroy_enclave(global_eid1);
        return false;
    }
  sealed_len=sealed_len*2;
  // allocate mem in sealed card with sealed_len size
  uint8_t *temp_sealed_buf = (uint8_t *)malloc(sealed_len);
    if(temp_sealed_buf == NULL)
    {
        std::cout << "Out of memory" << std::endl;
        sgx_destroy_enclave(global_eid1);
        return false;
    }

  uint8_t* client_id_bytes = (uint8_t*) malloc(sizeof(uint8_t) * client_id.length());
  memcpy(client_id_bytes,client_id.c_str(),client_id.length());  
    if((ret = be_init_card(global_eid1,card_bytes,card_size,temp_sealed_buf,sealed_len,sealed_len_p,client_id_bytes,client_id.length())) != SGX_SUCCESS)
  {

    print_error_message(ret,"be_init_card");
    return 1;
  }  


  FILE *file_ptr;
  std::string filename = client_id + ".bin";
  file_ptr = fopen(filename.c_str(),"wb");  // r for read, b for binary
  fwrite(temp_sealed_buf,1,sealed_len,file_ptr);
  fclose(file_ptr);
  
  return 0;
}




int do_validation(string client_id){
  sgx_status_t ret;
  uint8_t pos = NULL;
  uint8_t* pos_p = &pos;
  ostringstream ostrm;

  FILE *file_ptr;
  char filename[client_id.length()+5];
  strcpy(filename,client_id.c_str());
  strcat(filename,".bin");

  file_ptr = fopen(filename,"rb");  
  fseek(file_ptr, 0, SEEK_END);
  long file_size = ftell(file_ptr);
  fseek(file_ptr, 0, SEEK_SET);
  uint8_t* temp_buf = (uint8_t*) calloc(file_size,sizeof(uint8_t));
  fread(temp_buf, 1, file_size, file_ptr);
  fclose(file_ptr);

  
  uint8_t* sealed_card = temp_buf;
  size_t sealed_size = file_size;
  size_t* sealed_size_p = &sealed_size;


  int is_valid;
  is_valid=0;
  int* is_valid_p = &is_valid;
  int after_log_len = sealed_size*5;

  uint8_t *after_log = (uint8_t *)calloc(after_log_len,sizeof(uint8_t));
  size_t* after_log_len_p = (size_t*)&after_log_len;

  if((ret = be_validate(global_eid1,  
    temp_buf, 
    sealed_size,
    (uint8_t*) client_id.c_str(),
    (size_t)client_id.length(),
    is_valid_p,
    after_log,
    after_log_len,
    after_log_len_p
    )) != SGX_SUCCESS)
  {
    print_error_message(ret,"be_validate");
  }   

  file_ptr = fopen(filename,"wb");  // r for read, b for binary
  rewind(file_ptr);
  fwrite(after_log,1,*after_log_len_p,file_ptr);
  fclose(file_ptr);

  free(temp_buf);
  
return is_valid;

}


int menu(string client_id){
    int option;
    cout << "What do you want to do?" << endl;
    cout << "1. Generate new card" << endl;
    cout << "2. Parse card" << endl;
    cout << "3. Validate card" << endl;
    cout << "4. Get Card Logs" << endl;
    cout << "5. Exit" << endl;
    cout << "Option: ";
    cin >> option;

    // Check if the option is valid
    while (option < 1 || option > 5) {
        cout << "Invalid option. Try again: ";
        cin >> option;
    }
    cout << "\n" << endl;
    // For each option, call the corresponding function
    switch (option) {
        case 1:
            cout << "Generating card..." << endl;
            init_client(client_id);

            break;
        case 2:
            cout << "Parsing card..." << endl;
            //save_card();
            break;
        case 3:
            cout << "Validating card..." << endl;

            if (do_validation(client_id)){
              cout << "Correct!\n" << endl;
            }
            else{
              cout << "Incorrect!\n" << endl;
            }
            break;
        case 4:
        
            get_card_logs(client_id);
            break;
        case 5:
            cout << "Exiting..." << endl;
            return 0;
    }

    return 1;
}
/*
 * Application entry
 */

int SGX_CDECL main(int argc,char *argv[])
{
  unsigned long delta_t[10];
  int n = (argc == 1) ? 1000 : atoi(argv[1]);
  int a[n];
  int sum;
  sgx_status_t ret;

  /* initialize the array */
  if(n < 0 || n > 1000000) return 1;
  for(int i = 0;i < n;i++)
    a[i] = i;
  /* initialize the enclave and print welcome message */
  if(initialize_bankenclave() < 0)
    return 1; 

  // our code goes here

  // ""Login""
  string client_id;

  while (client_id == "") {
  printf("Enter Client ID: \n");
  getline(cin, client_id);
  }
  while (menu(client_id)){}


  /* destroy the enclave */
  if((ret = sgx_destroy_enclave(global_eid1)) != SGX_SUCCESS)
  {
    print_error_message(ret,"sgx_destroy_enclave");
    return 1;
  }
  return 0;
}

