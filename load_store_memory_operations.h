#ifndef LOAD_STORE_MEMORY_OPERATIONS_H
#define LOAD_STORE_MEMORY_OPERATIONS_H

#include <stdint.h>
#include "arm_core.h"
#include <stdbool.h>


/*
Return 0 sinon erreur
*/
int handle_memory_operation(arm_core p, uint32_t ins, uint32_t write_back);
int process_memory_access(arm_core p, uint32_t address, int load_store, int B, uint32_t Rd) ;
int process_memory_access_half(arm_core p, uint32_t address, uint32_t load_store, uint32_t Rd) ;
//------------------------------------------------------------------------------------------------

/*
Initialise les variables communes utilisees dans les fonctions de traitement des operations memoire
*/
void initialize_common_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B);


/*
Initialisent les variables specifiques pour les differents types d'operation memoire :
                                                (immediate, registre, scaled register).
*/
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


/*
ses fonctions return true si l'instruction correspond au type d'offset specifie, sinon false
*/
bool is_immediate_offset(uint32_t ins);
bool is_register_offset(uint32_t ins);
bool is_scaled_register_offset(uint32_t ins);

#endif