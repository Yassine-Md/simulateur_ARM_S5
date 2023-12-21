/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#ifndef __ARM_LOAD_STORE_H__
#define __ARM_LOAD_STORE_H__
#include <stdint.h>
#include "arm_core.h"

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
int handle_ldrh_strh(arm_core p, uint32_t ins);
int handle_immediate_post_indexed(arm_core p, uint32_t ins);
int handle_register_post_indexed(arm_core p, uint32_t ins);
int handle_scaled_register_post_indexed(arm_core p, uint32_t ins);
int handle_immediate_offset(arm_core p, uint32_t ins);
int handle_register_offset(arm_core p, uint32_t ins);
int handle_scaled_register_offset(arm_core p, uint32_t ins);
int handle_immediate_pre_indexed(arm_core p, uint32_t ins);
int handle_register_pre_indexed(arm_core p, uint32_t ins);
int handle_scaled_register_pre_indexed(arm_core p, uint32_t ins);
int handle_ldr_str(arm_core p, uint32_t ins);
int determine_addressing_mode(uint32_t ins);
int determine_instruction_type(uint32_t ins);
int arm_load_store(arm_core p, uint32_t ins);
int arm_load_store_multiple(arm_core p, uint32_t ins);
int arm_coprocessor_load_store(arm_core p, uint32_t ins);

#endif
