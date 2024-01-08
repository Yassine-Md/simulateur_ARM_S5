#ifndef LOAD_STORE_MEMORY_OPERATIONS_H
#define LOAD_STORE_MEMORY_OPERATIONS_H

#include <stdint.h>
#include "arm_core.h"


int handle_memory_operation(arm_core p, uint32_t ins, uint32_t write_back);
int process_memory_access_half(arm_core p, uint32_t address, uint32_t load_store, uint32_t Rd);
int process_memory_access_half(arm_core p, uint32_t address, uint32_t load_store, uint32_t Rd) ;

void initialize_common_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B);
void initialize_immediate_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rn, uint32_t *offset_12,
                          uint32_t* U, uint32_t *load_store, uint32_t *B);
void initialize_register_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rm, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B);
void initialize_scaled_register_variables(arm_core p, uint32_t ins,
                          uint32_t* shift, uint32_t* shift_imm,
                          uint32_t* Rd, uint32_t* Rm, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B);
                          
#endif