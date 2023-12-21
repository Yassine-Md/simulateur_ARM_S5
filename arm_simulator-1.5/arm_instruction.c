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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"


// Constantes pour les codes de condition
#define EQ 0
#define NE 1
#define CS 2
#define CC 3
#define MI 4
#define PL 5
#define VS 6
#define VC 7
#define HI 8
#define LS 9
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14
#define UNCONDITIONAL 15

// return 1 si la conditon et verifier 0 sinon
int ConditionPassed(arm_core p, uint32_t ins) {
    uint32_t cond = get_bits(ins, 31, 28);
    uint32_t cpsr = arm_read_cpsr(p);
    uint32_t z = get_bit(cpsr, Z);
    uint32_t c = get_bit(cpsr, C);
    uint32_t n = get_bit(cpsr, N);
    uint32_t v = get_bit(cpsr, V);

    switch (cond) {
        case EQ:
            return (z == 1) ? 1 : 0;
        case NE:
            return (z == 0) ? 1 : 0;
        case CS:
            return (c == 1) ? 1 : 0;
        case CC:
            return (c == 0) ? 1 : 0;
        case MI:
            return (n == 1) ? 1 : 0;
        case PL:
            return (n == 0) ? 1 : 0;
        case VS:
            return (v == 1) ? 1 : 0;
        case VC:
            return (v == 0) ? 1 : 0;
        case HI:
            return (c == 1 && z == 0) ? 1 : 0;
        case LS:
            return (c == 0 && z == 1) ? 1 : 0;
        case GE:
            return (n == v) ? 1 : 0;
        case LT:
            return (n != v) ? 1 : 0;
        case GT:
            return (z == 0 && ((n == 1 && v == 1) || (n == 0 && v == 0))) ? 1 : 0;
        case LE:
            return (z == 1 || (n == 1 && v == 0) || (n == 0 && v == 1)) ? 1 : 0;
        case AL:
            return 1;
        default: //UNCONDITIONAL
            return 1;
    }
}



static int arm_execute_instruction(arm_core p) {
    uint32_t value ; 
    int res = arm_fetch(p , &value);  // return 0 si tout est bien passer sinon 0
    if (res == 0 ){ // fetch bien passer
        uint8_t bit_21_20 = get_bits(value , 21 , 20) ;
        uint8_t bit_24_23 = get_bits(value , 24 , 23) ;
        uint8_t bit_4 = get_bit(value, 4);
        uint8_t bit_20 = get_bit(value , 20);
        uint8_t bit_7 = get_bit(value , 7);
        //reccuperer les bits de 25 a 27 de value pour avoir la categorie de l'instruction
        uint8_t cat_inst = get_bits(value , 27 , 25) ;  // categorie d'instruction
        if(ConditionPassed(p , value)){ // si les conditons sont verifier 
            switch(cat_inst){
                case 0:
                    if(bit_24_23 == 2 && bit_20 == 0){
                        // Miscellaneous instructions:
                        return arm_miscellaneous(p, value);  // pour mrs
                    } else if(bit_4 == 1 && bit_7 == 1){
                        return UNDEFINED_INSTRUCTION ;// undefine on va pas traiter les extra load store et les multiplies
                    } else {
                        return arm_data_processing_shift(p, value);
                    }
                case 1:
                    if ((bit_24_23 == 2)&& (bit_21_20 == 0)){
                        // Undifined instruction
                        return UNDEFINED_INSTRUCTION ;
                    }else{
                        //move immediate to status register  (mettre à jour le registre d'état avec une valeur immédiate)
                        //data prcessing immediate (effectuer des opérations arithmétiques ou logiques sur des données immédiates)
                        return arm_data_processing_shift(p ,value );
                    }
                case 2: // Load/store immediate offset
                    return arm_load_store(p, value);

                case 3: // Load/store register offset
                    if(bit_4 == 0){
                        return arm_load_store(p , value);
                    }else{
                        if(get_bits(value,24,20) == 0b11111){
                            // architecturally undefined
                            return UNDEFINED_INSTRUCTION;
                        }
                        else{
                            // media instruction 
                            return arm_coprocessor_others_swi(p , value);
                        }   
                    }
                case 4: // load store multiple
                    return arm_load_store_multiple(p, value );

                case 5: // branche with link 
                    return arm_branch(p, value );

                case 6: // Coprocessor load/store and double register transfers
                    return arm_coprocessor_load_store(p, value);
                
                case 7: 
                    if(get_bit(value , 24) == 0){
                        //coprocessor data processing
                        //coprocessor register transfers
                        return arm_coprocessor_others_swi(p , value);
                    }else{
                        // software interrupt
                        return INTERRUPT ;
                    }
            }
        }else{ // les conditions ne sont pas verifier
            return INTERRUPT ;
        }
    }
    //fetch mal passer 
    //appel a la fonction arm_exception ?
    return PREFETCH_ABORT ;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result) {
        return arm_exception(p, result);
    }
    return result;
}
