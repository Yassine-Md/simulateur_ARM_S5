#ifndef LOAD_STORE_ADDRESSING_OPERATIONS_H
#define LOAD_STORE_ADDRESSING_OPERATIONS_H

#include <stdint.h>
#include "arm_core.h"

// Define constants for addressing modes
#define IMMEDIATE_POST_INDEXED 10
#define REGISTER_POST_INDEXED 11
#define SCALED_REGISTER_POST_INDEXED 12
#define IMMEDIATE_OFFSET 13
#define REGISTER_OFFSET 14
#define SCALED_REGISTER_OFFSET 15
#define IMMEDIATE_PRE_INDEXED 16
#define REGISTER_PRE_INDEXED 17
#define SCALED_REGISTER_PRE_INDEXED 18

// Define constants for error
// UNDEFINED_INSTRUCTION 

/*
return : La fonction retourne l'adresse calculee 
*/
uint32_t calculate_address_scaled_register(arm_core p, uint32_t Rn, uint32_t Rm_value, uint32_t shift, uint32_t shift_imm, uint32_t U, uint32_t C_flag);

/*
Calcule une adresse en fonction des valeurs des registres et d'un offset
return : La fonction retourne l'adresse calculee 
*/
uint32_t calculate_address(arm_core p, uint32_t Rn, uint32_t offset, uint32_t U);

/*
retourne une constante representant le mode d'adressage pour les instructions LDRH/STRH.
*/
int determine_addressing_mode_ldrh_strh(uint32_t ins);

/*
retourne une constante representant le mode d'adressage pour les instructions LDR/STR.
*/
int determine_addressing_mode_ldr_str(uint32_t ins);

#endif 
