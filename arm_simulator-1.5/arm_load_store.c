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


uint32_t logical_shift_right(uint32_t value, uint8_t shift) {
    return value >> shift;
}

uint32_t logical_shift_left(uint32_t value, uint8_t shift) {
    return value << shift;
}


/*
P=1 et W=0 cela signifie que le registre de base reste inchangé après l'accès à la mémoire.
P=1 et W=1 cela signifie que le registre de base est mis à jour avec le résultat après l'accès à la mémoire
*/



// elle renvoie quoi cette fonction
int arm_load_store(arm_core p, uint32_t ins) {
    // reccuperer COND 
    uint8_t cond = get_bits (ins , 31 , 28);
    uint8_t load_store = get_bit(ins , 20); // 1 Load   0 Store
    uint8_t B = get_bit(ins , 22); // Distinguishes between an unsigned byte (B==1) and a word (B==0) access.
    uint8_t Rn = get_bits(ins , 19 , 16); // Specifies the base register used by <addressing_mode>.
    uint8_t Rd = get_bits(ins , 15 , 12); // Specifies the register whose contents are to be loaded or stored.
    uint8_t U = get_bit(ins , 23); 
    uint8_t half_W = get_bits(ins , 27 , 25); 
    uint8_t P = get_bit(ins , 24);
    uint8_t W = get_bit(ins , 21);
    uint32_t bit_11_4 = get_bits (ins , 11 , 4);
    uint8_t I = get_bit(ins , 25);
    uint32_t offset_12 = get_bits(ins , 11 ,0);
    uint8_t Rm = get_bits(ins , 3 ,0);
    uint16_t shift_imm = get_bits(ins , 11 , 7);
    uint16_t shift = get_bits(ins,6,5);
    uint8_t bit_4 = get_bit(ins,4);
    uint16_t flags = get_bits(p->cpsr , 31 , 28);
    uint8_t C_flag = get_bit (p->cpsr , 29);
    uint8_t bit_27_26 = get_bits (ins , 27 , 26);  // pour determiner si c'est un LDR/B STR/B | LDRH STRH
    uint8_t always = 15;


/*  utiliser ces fonction pour profiter des traces 
int arm_read_word(arm_core p, uint32_t address, uint32_t *value) // return boolean
int arm_read_byte(arm_core p, uint32_t address, uint8_t *value)  // return boolean
int arm_write_byte(arm_core p, uint32_t address, uint8_t value)  // return boolean 
int arm_write_word(arm_core p, uint32_t address, uint32_t value) // return boolean 

void arm_write_register(arm_core p, uint8_t reg, uint32_t value) //return void modifie la valeur du registre Rreg = value
uint32_t arm_read_register(arm_core p, uint8_t reg) // return Rreg value 
*/

/*
if (load_store == 0){ // Store
    if(B == 0){ // Word
                                    
    }else{ //Byte
            
    }
    
}else{ // Load
    if(B == 0){ // Word

    }else{ //Byte

    }

}


*/

    uint32_t memory ;
    uint32_t address ;
    uint32_t value ;
    uint32_t index ;

    if (bit_27_26 == 1){ //  LDR/B STR/B 
        if (P == 0){
            if(W == 0){
                if(I == 0){
                    // immediate post-indexed
                    address = arm_read_register(p, Rn);
                    if(cond == flags){
                        if(U == 1){
                            value = arm_read_register(p, Rn) + offset_12 ;
                            arm_write_register(p, Rn, value);
                        }else{
                            value = arm_read_register(p, Rn) - offset_12 ;
                            arm_write_register(p, Rn, value);
                        }
                    }else{ // si l'instruction ne verifie pas les condtion 
                        //Exception
                    }
                }else{  // I == 1
                    if(bit_11_4 == 0){
                        //register post indexed
                        address = arm_read_register(p, Rn);
                        if(cond == flags){
                            if(U == 1){
                                value = arm_read_register(p, Rn) + arm_read_register(p, Rm) ;
                                arm_write_register(p, Rn, value);
                            }else{
                                value = arm_read_register(p, Rn) - arm_read_register(p, Rm);
                                arm_write_register(p, Rn, value);
                            }
                        }
                    }
                    else if(bit_4 == 0){
                        // scaled register post-indexed
                        address = arm_read_register(p , Rn);
                        switch (shift){
                            case 0:
                                index = logical_shift_left (arm_read_register(p , Rm) , shift_imm);
                                break;
                            case 1:
                                if(shift_imm == 0){
                                    index = 0;
                                }else{
                                    index = logical_shift_right(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            case 2:
                                if(shift_imm == 0){
                                    if(get_bit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else{
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3:
                                if(shift_imm == 0){
                                    index = logical_shift_left(C_flag , 31) | logical_shift_right(arm_read_register(p , Rm) , 1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                        }
                        if(cond == flags){
                            if(U == 1){
                                value = arm_read_register(p, Rn) + index;
                                arm_write_register(p, Rn, value);
                            }else{
                                value = arm_read_register(p, Rn) - index;
                                arm_write_register(p, Rn, value);
                            }
                        }
                        
                    }else{
                        // undefined instruction 
                    }
                }
            }else{ // W == 1
                // undefined instruction  p459 privilege mode strt ldrt ...
            }

        }else{ // P == 1
            if (W == 0){
                if(I == 0){
                    // immediate offset
                    if (U == 1){
                        address = arm_read_register(p , Rn) + offset_12 ;
                    }else{
                        address= arm_read_register(p , Rn) - offset_12 ;
                    }
                // load ou store (byte or word) manipulation 

                }else{ // I == 1
                    if(bit_11_4 == 0){
                            
                        //register offset 
                        if (U == 1){
                            address  = arm_read_register(p , Rn) + arm_read_register(p , Rm) ;
                        }else{
                            address  = arm_read_register(p , Rn) - arm_read_register(p , Rm) ;
                        }
                    }
                    else if(bit_4 == 0){
                        // scaled register offset 
                        switch (shift){
                            case 0:
                                index = logical_shift_left(arm_read_register(p , Rm) , shift_imm) ; 
                            case 1:
                                if (shift_imm == 0){
                                    index = 0 ;
                                }else{
                                    logical_shift_right(arm_read_register(p , Rm) , shift_imm) ;
                                }
                                break ;
                            case 2:
                                if(shift_imm == 0){
                                    if(getbit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else {
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3: 
                                if(shift_imm == 0){
                                    index  = logical_shift_left(C_flag, 31) | logical_shift_right(arm_read_register(p , Rm) ,1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            case default :
                                break; // return undefined 
                        }
                        if(U == 1){
                            address = arm_read_register(p , Rn) + index ;
                        }else{
                            address = arm_read_register(p , Rn) - index ;
                        }
                    }else{
                        // undefined instruction 
                    }

                }
            }else{ // W == 1
                if(I == 0){
                //immediate pre-indexed
                    if(U == 1){
                        address = arm_read_register(p , Rn) + offset_12 ;
                    }else{
                        address = arm_read_register(p , Rn) - offset_12 ;
                    }
                    // si cond == au flags ZNCV de CPSR
                    if (cond == flags){
                        arm_write_register(p , Rn , address);
                    }

                }else{ // I == 1
                    if(bit_11_4 == 0){
                        //register pre-indexed
                        if(U == 1){
                            address = arm_read_register(p , Rn) + arm_read_register(p , Rm) ;
                        }else{
                            address = arm_read_register(p , Rn) - arm_read_register(p , Rm) ;
                        }
                        if (cond == flags){
                            arm_write_register(p , Rn , address);
                        }
                    }
                    else if(bit_4 == 0){
                        //scaled register pre-indexed
                        switch (shift){
                            case 0:
                                index = logical_shift_left(arm_read_register(p , Rm) , shift_imm) ; 
                            case 1:
                                if (shift_imm == 0){
                                    index = 0 ;
                                }else{
                                    logical_shift_right(arm_read_register(p , Rm) , shift_imm) ;
                                }
                                break ;
                            case 2:
                                if(shift_imm == 0){
                                    if(getbit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else {
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3: 
                                if(shift_imm == 0){
                                    index  = logical_shift_left(C_flag, 31) | logical_shift_right(arm_read_register(p , Rm) ,1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            case default :
                                break; // return undefined 
                        }
                        if(U == 1){
                            address = arm_read_register(p , Rn) + index ;
                        }else{
                            address = arm_read_register(p , Rn) - index ;
                        }
                        if(cond == flags){
                            arm_write_register(p , Rn , address);
                        }
                        
                    }
                    else{
                        //undefined 
                    }  
                }
            }

        }

    }else{ //LDRH STRH

    }

    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
