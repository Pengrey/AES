#include "BankEnclave_u.h"
#include <errno.h>

typedef struct ms_e1_sum_array_t {
	int* ms_ptr;
	size_t ms_n;
	int* ms_sum;
} ms_e1_sum_array_t;

typedef struct ms_be_init_card_t {
	int** ms_card;
	size_t ms_n;
} ms_be_init_card_t;

typedef struct ms_ocall_e1_print_string_t {
	const char* ms_str;
} ms_ocall_e1_print_string_t;

static sgx_status_t SGX_CDECL BankEnclave_ocall_e1_print_string(void* pms)
{
	ms_ocall_e1_print_string_t* ms = SGX_CAST(ms_ocall_e1_print_string_t*, pms);
	ocall_e1_print_string(ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_BankEnclave = {
	1,
	{
		(void*)BankEnclave_ocall_e1_print_string,
	}
};
sgx_status_t e1_sum_array(sgx_enclave_id_t eid, int* ptr, size_t n, int* sum)
{
	sgx_status_t status;
	ms_e1_sum_array_t ms;
	ms.ms_ptr = ptr;
	ms.ms_n = n;
	ms.ms_sum = sum;
	status = sgx_ecall(eid, 0, &ocall_table_BankEnclave, &ms);
	return status;
}

sgx_status_t be_init_card(sgx_enclave_id_t eid, int** card, size_t n)
{
	sgx_status_t status;
	ms_be_init_card_t ms;
	ms.ms_card = card;
	ms.ms_n = n;
	status = sgx_ecall(eid, 1, &ocall_table_BankEnclave, &ms);
	return status;
}

