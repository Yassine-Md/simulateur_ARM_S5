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
#include "util.h"
#include "debug.h"





/*
P=1 et W=0 cela signifie que le registre de base reste inchangé après l'accès à la mémoire.
P=1 et W=1 cela signifie que le registre de base est mis à jour avec le résultat après l'accès à la mémoire
*/


/* A voir pour ajouter des conditions de verification
https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-instruction-set/memory-access-instructions/ldr-and-str--register-offset
*/

// les fonctions handel doit renvoyer 


// Define constants for instruction types
#define LDR_STR_INSTRUCTION 1
#define LDRH_STRH_INSTRUCTION 2

// Define constants for addressing modes
#define IMMEDIATE_POST_INDEXED 0
#define REGISTER_POST_INDEXED 1
#define SCALED_REGISTER_POST_INDEXED 2
#define IMMEDIATE_OFFSET 3
#define REGISTER_OFFSET 4
#define SCALED_REGISTER_OFFSET 5
#define IMMEDIATE_PRE_INDEXED 6
#define REGISTER_PRE_INDEXED 7
#define SCALED_REGISTER_PRE_INDEXED 8

// Define constants for error
// UNDEFINED_INSTRUCTION 

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
            return 99;
    }

    return calculate_address(p, Rn, index, U);
}

int handle_immediate_common(arm_core p, uint32_t ins, uint32_t Rd, uint32_t Rn, uint32_t U, uint32_t load_store) {
    uint32_t immedL = get_bits(ins, 3, 0);
    uint32_t immedH = get_bits(ins, 11, 8);
    uint32_t offset_8 = logical_shift_left(immedH, 4) | immedL;
    uint32_t address = calculate_address(p, Rn, offset_8, U);
    if (get_bits(ins,22,21) == 0x11 || get_bits(ins,22,21) == 0x10){
        arm_write_register(p, Rn, address);                            //handle_immediate_offset_ldrh_strh pour cette fonction il n'y a pas une ecreture de la nouvelle adresse calculer dans Rn
    }
    return process_memory_access_half(p, address, load_store, Rd);
}


int determine_addressing_mode(uint32_t ins){
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

// fonction pour faire les accees memoire et recuperer les valeurs soit pour effectuer le ldr soit le str
int process_memory_access(arm_core p, uint32_t address, int load_store, int B, uint32_t Rd, uint32_t *chargedValue_word, uint8_t *chargedValue_byte) {
    if (load_store == 0) { // Store
        if (B == 0) { // Word
            return arm_write_word(p, address, arm_read_register(p, Rd));
        } else { // Byte
            return arm_write_byte(p, address, (uint8_t)arm_read_register(p, Rd));
        }
    } else { // Load
        if (B == 0) { // Word
            arm_read_word(p, address, chargedValue_word);
            return arm_write_register(p, Rd, *chargedValue_word);
        } else { // Byte
            arm_read_byte(p, address, chargedValue_byte);
            return arm_write_register(p, Rd, *chargedValue_byte);
        }
    }
}

int process_memory_access_half(arm_core p, uint32_t address, uint32_t load_store, uint32_t Rd) {
    uint16_t valueToWriteOrRead;

    if (load_store == 0) { // store
        valueToWriteOrRead = (uint16_t)arm_read_register(p, Rd);
        return arm_write_half(p, address, valueToWriteOrRead);
    } else { // ldr
        arm_read_half(p, address, &valueToWriteOrRead);
        return arm_write_register(p, Rd, valueToWriteOrRead);
    }
}

int handle_ldr_str(arm_core p, uint32_t ins) {
    int addressing_mode = determine_addressing_mode(ins);

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


int handle_scaled_register_pre_indexed(arm_core p, uint32_t ins) {
    uint32_t shift, shift_imm, Rd, Rm, Rn, U, load_store, B;
    uint32_t C_flag = get_bit(arm_read_cpsr(p), 29);
    initialize_scaled_register_variables(p, ins, &shift, &shift_imm, &Rd, &Rm, &Rn, &U, &load_store, &B);

    uint32_t chargedValue_word , address;
    uint8_t chargedValue_byte ;

    address = calculate_address_scaled_register(p, Rn, arm_read_register(p , Rm), shift, shift_imm, U, C_flag);
    arm_write_register(p, Rn, address);

    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}


int handle_register_pre_indexed(arm_core p, uint32_t ins) {
    
    uint32_t Rd, Rm, Rn, U, load_store, B;
    initialize_register_variables(p, ins, &Rd, &Rm, &Rn, &U, &load_store, &B);

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;
    
    address = calculate_address(p, Rn, arm_read_register(p , Rm), U);
    arm_write_register(p , Rn , address);

    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}

int handle_immediate_pre_indexed(arm_core p, uint32_t ins) {

    uint32_t Rd, offset_12, Rn, U, load_store, B;
    initialize_immediate_variables(p, ins, &Rd, &Rn, &offset_12, &U, &load_store, &B);


    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address(p, Rn, offset_12, U);
    arm_write_register(p , Rn , address);

    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);

}

int handle_scaled_register_offset(arm_core p, uint32_t ins) {

    uint32_t shift, shift_imm, Rd, Rm, Rn, U, load_store, B;
    uint32_t C_flag = get_bit (arm_read_cpsr(p) , 29);
    initialize_scaled_register_variables( p,  ins, &shift, &shift_imm,
                          &Rd, &Rm, &Rn,&U, &load_store, &B) ;

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address_scaled_register(p, Rn, arm_read_register(p , Rm), shift, shift_imm, U, C_flag);
    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}

int handle_register_offset(arm_core p, uint32_t ins) {
    
    uint32_t Rd, Rm, Rn, U, load_store, B;
    initialize_register_variables(p, ins, &Rd, &Rm, &Rn, &U, &load_store, &B);

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address(p, Rn, arm_read_register(p , Rm), U);
   
    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}

int handle_immediate_offset(arm_core p, uint32_t ins) {

    uint32_t Rd, Rn, U, load_store, B , offset_12;
    initialize_immediate_variables(p, ins, &Rd, &Rn, &offset_12,
                                        &U, &load_store, &B);
    
    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address(p, Rn, offset_12, U);

    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}

int handle_scaled_register_post_indexed(arm_core p, uint32_t ins) {

    uint32_t shift, shift_imm, Rd, Rm, Rn, U, load_store, B;
    uint32_t C_flag = get_bit (arm_read_cpsr(p) , 29);
    initialize_scaled_register_variables( p,  ins, &shift, &shift_imm,
                          &Rd, &Rm, &Rn,&U, &load_store, &B) ;

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address_scaled_register(p, Rn, arm_read_register(p , Rm), shift, shift_imm, U, C_flag);

    arm_write_register(p, Rn, address);
    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);
}

int handle_register_post_indexed(arm_core p, uint32_t ins) {
    uint32_t Rd, Rm, Rn, U, load_store, B;
    initialize_register_variables(p, ins, &Rd, &Rm, &Rn, &U, &load_store, &B);

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address(p, Rn, arm_read_register(p, Rm), U);

    arm_write_register(p, Rn, address);
    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);

}

int handle_immediate_post_indexed(arm_core p, uint32_t ins) {

    uint32_t Rd, Rn, U, load_store, B , offset_12;
    initialize_immediate_variables(p, ins, &Rd, &Rn, &offset_12,
                                        &U, &load_store, &B);

    uint32_t chargedValue_word ;
    uint8_t chargedValue_byte ;
    uint32_t address ;

    address = calculate_address(p, Rn, offset_12, U);
    arm_write_register(p, Rn, address);
    return process_memory_access(p, address, load_store, B, Rd, &chargedValue_word, &chargedValue_byte);

}

int handle_immediate_offset_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
        return handle_immediate_common(p, ins, Rd, Rn, U, load_store) ;
}


int handle_register_offset_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    uint32_t Rm = get_bits(ins , 3 , 0);
    uint32_t address = calculate_address(p, Rn, arm_read_register(p, Rm), U);
    return process_memory_access_half(p, address, load_store, Rd);
}


int handle_immediate_pre_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_immediate_common(p, ins, Rd, Rn, U, load_store) ;
}


int handle_register_pre_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    uint32_t Rm = get_bits(ins , 3 , 0);
    uint32_t address = calculate_address(p, Rn, arm_read_register(p, Rm), U);
    arm_write_register(p , Rn , address);
    return process_memory_access_half(p, address, load_store, Rd);

}

int handle_immediate_post_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    return handle_immediate_common(p, ins, Rd, Rn, U, load_store) ;
}


int handle_register_post_indexed_ldrh_strh(arm_core p , uint32_t ins , uint32_t Rd , uint32_t Rn , uint32_t U , uint32_t load_store){
    uint32_t Rm = get_bits(ins , 3 , 0);
    uint32_t address = calculate_address(p, Rn, arm_read_register(p, Rm), U);
    arm_write_register(p , Rn , address);
    return process_memory_access_half(p, address, load_store, Rd);
}


int handle_ldrh_strh(arm_core p, uint32_t ins) {
    uint32_t Rd = get_bits(ins , 15 , 12);
    uint32_t Rn = get_bits(ins , 19 , 16);
    uint32_t B = get_bit(ins , 22);
    uint32_t U = get_bit(ins , 23);
    uint32_t load_store = get_bit(ins , 20);

    // faut il mettre a jour Rn apres avoir calculer adresse ?? 
    if(B == 1){ // immediate offset-index
        return handle_register_offset_ldrh_strh(p , ins , Rd , Rn , U , load_store);
    }else{ // Register ossfet-index
        return handle_immediate_offset_ldrh_strh(p , ins , Rd , Rn , U , load_store);
    }
}


// Common initialization function
void initialize_common_variables(arm_core p, uint32_t ins,
                          uint32_t *Rd, uint32_t *Rn,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    *Rd = get_bits(ins, 15, 12);
    *Rn = get_bits(ins, 19, 16);
    *U = get_bit(ins, 23);
    *load_store = get_bit(ins, 20);
    *B = get_bit(ins, 22);
}

void initialize_immediate_variables(arm_core p, uint32_t ins,
                          uint32_t *Rd, uint32_t *Rn, uint32_t *offset_12,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    initialize_common_variables(p,ins,Rd,Rn,U,load_store,B);
    *offset_12 = get_bits(ins , 11 , 0);
}

void initialize_register_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rm, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B) {
    initialize_common_variables(p,ins,Rd,Rn,U,load_store,B);
    *Rm = get_bits(ins, 3, 0);
}

void initialize_scaled_register_variables(arm_core p, uint32_t ins,
                          uint32_t *shift, uint32_t *shift_imm,
                          uint32_t *Rd, uint32_t *Rm, uint32_t *Rn,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    initialize_register_variables(p,ins,Rd,Rm,Rn,U,load_store,B);
    *shift = get_bits(ins, 6, 5);
    *shift_imm = get_bits(ins, 11, 7);
}


//*c'est l'exepetion DATA_ABORT
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
