
#include "arm_exception.h"
#include "arm_constants.h"
#include "load_store_addressing_operations.h"
#include "util.h"
#include "debug.h"
#include <stdlib.h>


uint32_t calculate_address(arm_core p, uint32_t Rn, uint32_t offset, uint32_t U) {
    uint32_t address = arm_read_register(p,Rn);
    if (U == 1) {
        return address + offset;
    } else {
        return address - offset;
    }
}



// Fonction auxiliaire pour calculer l'adresse en fonction de U, Rn et index
uint32_t calculate_address_scaled_register(arm_core p, uint32_t Rn, uint32_t Rm_value, uint32_t shift, uint32_t shift_imm, uint32_t U, uint32_t C_flag) {
    uint32_t index;

    switch (shift) {
        case 0:
            index = logical_shift_left(Rm_value, shift_imm);
            break;
        case 1:
            if (shift_imm == 0) {
                index = 0;
            } else {
                index = logical_shift_right(Rm_value, shift_imm);
            }
            break;
        case 2:
            if (shift_imm == 0) {
                index = (get_bit(Rm_value, 31) == 1) ? 0xFFFFFFFF : 0;
            } else {
                index = asr(Rm_value, shift_imm);
            }
            break;
        case 3:
            if (shift_imm == 0) {
                index = logical_shift_left(C_flag, 31) | logical_shift_right(Rm_value, 1);
            } else {
                index = ror(Rm_value, shift_imm);
            }
            break;
        default:
            // Handle undefined case
            printf("ERROR 'calculate_address_scaled_register' Undefined case \n");
            exit(1);
    }

    return index;
}


int determine_addressing_mode_ldr_str(uint32_t ins){
    uint32_t P = get_bit(ins , 24);
    uint32_t W = get_bit(ins , 21);
    uint32_t I = get_bit(ins , 25);
    uint32_t bit_11_4 = get_bits (ins , 11 , 4);
    uint32_t bit_4 = get_bit(ins,4);

    if(P == 0){
        if(W == 0){
            if(I == 0){
                return IMMEDIATE_POST_INDEXED ;
            }else{ // I == 1
                if(bit_11_4 == 0){
                    return REGISTER_POST_INDEXED ;
                }
                if(bit_4 == 0){
                    return SCALED_REGISTER_POST_INDEXED;
                }
                // si aucune de ces deux conditions n'est verifier 
                return UNDEFINED_INSTRUCTION ;
            }
        }else{ // W == 1
            // undefined instruction  p459 privilege mode strt ldrt ...
            return UNDEFINED_INSTRUCTION ;
        }
    }else{ // P == 1
        if(W == 0){
            if(I == 0){
                return IMMEDIATE_OFFSET ;
            }else{ // I == 1
                if(bit_11_4 == 0){
                    return REGISTER_OFFSET ;
                }
                if(bit_4 == 0){
                    return SCALED_REGISTER_OFFSET ;
                }
                // si aucune de ces deux conditions n'est verifier 
                return UNDEFINED_INSTRUCTION ;
            }
        }else{ // W == 1
            if(I == 0){
                return IMMEDIATE_PRE_INDEXED ;
            }else{ // I == 1
                if(bit_11_4 == 0){
                    return REGISTER_PRE_INDEXED ;
                }
                if(bit_4 == 0){
                    return SCALED_REGISTER_PRE_INDEXED ;
                }
                // si aucune de ces deux conditions n'est verifier 
                return UNDEFINED_INSTRUCTION ;
            }

        }
    }
}


int determine_addressing_mode_ldrh_strh(uint32_t ins){
    if(get_bits(ins,27,24) == 0x0000){
        if(get_bits(ins,22,21) == 0x10){
            return IMMEDIATE_POST_INDEXED;
        }else if(get_bits(ins,22,21) == 0x00){
            return REGISTER_POST_INDEXED;
        }else{
            return UNDEFINED_INSTRUCTION ;
        }
    }else if (get_bits(ins,27,24) == 0x0001){
        if(get_bits(ins,22,21) == 0x10){
            return IMMEDIATE_OFFSET;
        }else if(get_bits(ins,22,21) == 0x00){
            return REGISTER_OFFSET;
        }else if(get_bits(ins,22,21) == 0x11){
            return IMMEDIATE_PRE_INDEXED;
        }else if(get_bits(ins,22,21) == 0x01){
            return REGISTER_PRE_INDEXED;
        }else{
            return UNDEFINED_INSTRUCTION ;
        }
    }else{
        return UNDEFINED_INSTRUCTION;
    }
}