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







// LDM : load multiple 




int arm_load_store(arm_core p, uint32_t ins) {
    // reccuperer COND 
    uint8_t cond = get_bits (ins , 31 , 28);
    uint8_t L_S = get_bit(ins , 20); // 1 Load   0 Store
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


    w = 21 
    u = 23 
    p = 24 
    i = 25

    LDRH 27 26 25 = 000 b= 1

    if (L_S){
        // LOAD

        if (B) {
            //LDRB
            // scaled register offset 
            if((I == 1) && (P == 1) && (W == 0) && (bit_4 == 0)){
                switch (shift){
                    case 0:
                        // index = Rm Logical_Shift_Left shift_imm
                    case 1:
                        /*
                        if shift_imm == 0 then /* LSR #32 /
                            index = 0
                        else
                            index = Rm Logical_Shift_Right shift_imm
                        */
                    case 2:
                        /*
                        if shift_imm == 0 then /* ASR #32/
                        if Rm[31] == 1 then
                        index = 0xFFFFFFFF
                        else
                        index = 0
                        else
                        index = Rm Arithmetic_Shift_Right shift_imm
                        */
                    case 3:
                }
            }

            //register offset 
            if((I == 1) && (P == 1) && (W == 0) && (bit_11_4 == 0) ){
                if (U == 1){
                    p->memory = p->reg[Rn] + p->reg[Rm] ;
                }else{
                    p->memory = p->reg[Rn] - p->reg[Rm] ;
                }
            }
            // immediate offset
            if((I == 0) && (P == 1) && (W == 0) ) {
                if (U == 1){
                    p->memory = p->reg[Rn] + offset_12 ;
                }else{
                    p->memory = p->reg[Rn] - offset_12 ;
                }
            }



            //LDRH

        }else{
            // LDR
            // immediate offset
            if((I == 0) && (P == 1) && (W == 0) ) {
                if (U == 1){
                    p->memory = p->reg[Rn] + offset_12 ;
                }else{
                    p->memory = p->reg[Rn] - offset_12 ;
                }
            }

            

        }

        //immediate offset 

        //register offset 

        // scaled register offset 

        //immediate offset 




    }else{
        // store 
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
