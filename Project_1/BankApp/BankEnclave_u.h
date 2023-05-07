#ifndef BANKENCLAVE_U_H__
#define BANKENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCALL_E1_PRINT_STRING_DEFINED__
#define OCALL_E1_PRINT_STRING_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_e1_print_string, (const char* str));
#endif

sgx_status_t e1_printf_hello_world(sgx_enclave_id_t eid);
sgx_status_t e1_sum_array(sgx_enclave_id_t eid, int* ptr, size_t n, int* sum);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
