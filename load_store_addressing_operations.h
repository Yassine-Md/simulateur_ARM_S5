#ifndef LOAD_STORE_ADDRESSING_OPERATIONS_H
#define LOAD_STORE_ADDRESSING_OPERATIONS_H

#include <stdint.h>
#include "arm_core.h"

uint32_t calculate_address_scaled_register(arm_core p, uint32_t Rn, uint32_t Rm_value, uint32_t shift, uint32_t shift_imm, uint32_t U, uint32_t C_flag);
uint32_t calculate_address(arm_core p, uint32_t Rn, uint32_t offset, uint32_t U);
int determine_addressing_mode_ldrh_strh(uint32_t ins);
int determine_addressing_mode_ldr_str(uint32_t ins);

#endif 
