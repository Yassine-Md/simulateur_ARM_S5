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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "load_store_addressing_operations.h"
#include "util.h"
#include "debug.h"
#include "load_store_memory_operations.h"



/* A voir pour ajouter des conditions de verification
https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-instruction-set/memory-access-instructions/ldr-and-str--register-offset
*/


// Define constants for instruction types
#define LDR_STR_INSTRUCTION 1
#define LDRH_STRH_INSTRUCTION 2


// return 0 si tout est bien passer sinon i#0
int arm_load_store(arm_core p, uint32_t ins) {
    int instruction_type = determine_instruction_type(ins);

    switch (instruction_type) {
        case LDR_STR_INSTRUCTION:
            return handle_ldr_str(p, ins);
        case LDRH_STRH_INSTRUCTION:
            return handle_ldrh_strh(p, ins);
        default:
            return UNDEFINED_INSTRUCTION;
    }
}

int determine_instruction_type(uint32_t ins) {  
    uint32_t type = get_bits(ins , 27 , 26);
    if(type == 1){
        return LDR_STR_INSTRUCTION ;
    }
    if(type == 0){
        return LDRH_STRH_INSTRUCTION ;
    }else{
        return  UNDEFINED_INSTRUCTION ;//ERROR ;
    }

}

// return 0 sinon -1 en cas d'erreur 
int handle_immediate_common_ldrh_strh(arm_core p, uint32_t ins, uint32_t Rd, uint32_t Rn, uint32_t U, uint32_t load_store , uint32_t write_back) {
    uint32_t immedL = get_bits(ins, 3, 0);
    uint32_t immedH = get_bits(ins, 11, 8);
    uint32_t offset_8 = logical_shift_left(immedH, 4) | immedL;
    uint32_t address = calculate_address(p, Rn, offset_8, U);
    if (write_back){
        arm_write_register(p, Rn, address);                            
    }
    return process_memory_access_half(p, address, load_store, Rd);
}

int handle_register_common_ldrh_strh(arm_core p, uint32_t ins, uint32_t Rd, uint32_t Rn, uint32_t U, uint32_t load_store, uint32_t write_back) {
    uint32_t Rm = get_bits(ins, 3, 0);
    uint32_t address = calculate_address(p, Rn, arm_read_register(p, Rm), U);
    if (write_back) {
        arm_write_register(p, Rn, address);
    }

    return process_memory_access_half(p, address, load_store, Rd);
}


int handle_ldr_str(arm_core p, uint32_t ins) {
    int addressing_mode = determine_addressing_mode_ldr_str(ins);

    switch (addressing_mode) {
        case IMMEDIATE_POST_INDEXED:
            return handle_immediate_post_indexed(p, ins);
        case REGISTER_POST_INDEXED:
            return handle_register_post_indexed(p, ins);
        case SCALED_REGISTER_POST_INDEXED:
            return handle_scaled_register_post_indexed(p, ins);
        case IMMEDIATE_OFFSET:
            return handle_immediate_offset(p, ins);
        case REGISTER_OFFSET:
            return handle_register_offset(p, ins);
        case SCALED_REGISTER_OFFSET:
            return handle_scaled_register_offset(p, ins);
        case IMMEDIATE_PRE_INDEXED:
            return handle_immediate_pre_indexed(p, ins);
        case REGISTER_PRE_INDEXED:
            return handle_register_pre_indexed(p, ins);
        case SCALED_REGISTER_PRE_INDEXED:
            return handle_scaled_register_pre_indexed(p, ins);
        default:
            return UNDEFINED_INSTRUCTION;
    }
}

int handle_ldrh_strh(arm_core p, uint32_t ins) {
    int addressing_mode = determine_addressing_mode_ldr_str(ins);
    uint32_t Rd, Rn, U, load_store;
    initialize_common_variables(p, ins, &Rd, &Rn, &U, &load_store, NULL) ;

    switch (addressing_mode) {
        case IMMEDIATE_POST_INDEXED:
            return handle_immediate_post_indexed_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        case REGISTER_POST_INDEXED:
            return handle_register_post_indexed_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        case IMMEDIATE_OFFSET:
            return handle_immediate_offset_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        case REGISTER_OFFSET:
            return handle_register_offset_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        case IMMEDIATE_PRE_INDEXED:
            return handle_immediate_pre_indexed_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        case REGISTER_PRE_INDEXED:
            return handle_register_pre_indexed_ldrh_strh(p , ins , Rd , Rn , U , load_store);
        default:
            return UNDEFINED_INSTRUCTION;
    }
}

int handle_scaled_register_pre_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}


int handle_register_pre_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}

int handle_immediate_pre_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}

int handle_scaled_register_offset(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 0);
}

int handle_register_offset(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 0);
}

int handle_immediate_offset(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 0);
}

int handle_scaled_register_post_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}

int handle_register_post_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}

int handle_immediate_post_indexed(arm_core p, uint32_t ins) {
    return handle_memory_operation(p, ins, 1);
}

int handle_immediate_offset_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
        return handle_immediate_common_ldrh_strh(p, ins, Rd, Rn, U, load_store,0) ;
}


int handle_register_offset_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_register_common_ldrh_strh(p, ins, Rd, Rn, U, load_store, 0);
}


int handle_immediate_pre_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_immediate_common_ldrh_strh(p, ins, Rd, Rn, U, load_store,1) ;
}


int handle_register_pre_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_register_common_ldrh_strh(p, ins, Rd, Rn, U, load_store, 1);
   }



int handle_immediate_post_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_immediate_common_ldrh_strh(p, ins, Rd, Rn, U, load_store,1) ;
}


int handle_register_post_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_register_common_ldrh_strh(p, ins, Rd, Rn, U, load_store, 1);
}


//pour le load/store normal on va utiliser les commande LDM et STM */
int arm_load_store_multiple(arm_core p, uint32_t ins) {
    //uint8_t cond = get_bits(ins,31,28);
    uint8_t P = get_bit(ins,24);//indicateur du mode d'adressage 1:mode post_increment(adresses sont incrémentées après le transfert)
    // 2:mode pre_increment(adresses sont décrémentées avant le transfert)
    uint8_t U = get_bit(ins,23);//bit U spécifie la direction de l'incrémentation/decrémentation des adresses. S'il est à 1, les adresses sont 
    //incrémentées. S'il est à 0, les adresses sont décrémentées.
    //uint8_t W = get_bit(ins,21);//si W==1 le registre Rn sera mis a jour 0 si il reste inchagé
    //bit 22 il doit etre 0 
    uint8_t Rn = get_bits(ins,19,16);
    uint16_t reg_list=get_bits(ins,15,0);
    if (get_bit(ins, 20 )==0){
        //STM
        uint32_t address=arm_read_register(p,(int) Rn);
        for (int i=0;i<15;i++){
            if (get_bit(reg_list,i)==1){
                if (P==1){
                    if (U==1){
                        address=address+4;
                    }
                    else{
                    address=address-4;
                    }
                }
                uint32_t val = arm_read_register(p,i);
                arm_write_word(p,address,val);
                if(P==0){ 
                    if (U==1){
                        address=address+4;
                    }
                    else{
                        address=address-4;
                    }
                }
            }
        }

    }
    else{
        //LDM
        uint32_t address=arm_read_register(p,(int) Rn);
        for (int i=0 ;i<15;i++){
            if (get_bit(reg_list,i)==1){
                if (P==1){
                    if (U==1){
                        address=address+4;
                    }
                    else{
                    address=address-4;
                    }
                }
                uint32_t value;
                arm_read_word(p,address,&value);
                arm_write_register(p,i,value);
                if(P==0){ 
                    if (U==1){
                        address=address+4;
                    }
                    else{
                        address=address-4;
                    }
                }
            }
        }
    }
    return DATA_ABORT;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
