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

static int arm_execute_instruction(arm_core p) {
    uint32_t value ; 
    int res = arm_fetch(p , &value);  // return 0 si tout est bien passer sinon 0
    if (res == 0 ){
        uint8_t bit_21_20 = get_bits(value , 21 , 20) ;
        uint8_t bit_24_23 = get_bits(value , 24 , 23) ;
        uint8_t bit_4 = get_bit(value, 4);
        uint8_t bit_20 = get_bit(value , 20);
        //reccuperer les bits de 25 a 27 de value pour avoir la categorie de l'instruction
        uint8_t cat_inst = get_bits(value , 27 , 25) ;  // categorie d'instruction
        switch(cat_inst){
            case 0:     
                if(bit_24_23 == 2 && bit_20 == 0){
                    // Miscellaneous instructions:
                    arm_miscellaneous(p, value);
                }else{
                    if(bit_4 == 1){
                    arm_data_processing_immediate_msr(p , value);
                    }else{
                    arm_data_processing_shift(p,value);
                    }
                }
                // extra load/store 
                // a voir 
                
            case 1:
                if ((bit_24_23 == 2)&& (bit_21_20 == 2)){
                    // Undifined instruction
                    arm_coprocessor_others_swi(p , value);
                }else{
                    //move immediate to status register  (mettre à jour le registre d'état avec une valeur immédiate)
                    //data prcessing immediate (effectuer des opérations arithmétiques ou logiques sur des données immédiates)
                    arm_data_processing_immediate_msr(p ,value );
                }
            case 2: // Load/store immediate offset
                arm_load_store(p, value);

            case 3: // Load/store register offset
                if(bit_4 == 1 ){
                    // media instruction 
                    // architecturally undefined
                    arm_coprocessor_others_swi(p , value);
                }
                arm_load_store(p, value);

            case 4: // load store multiple
                arm_load_store_multiple(p, value );

            case 5: // branche with link 
                arm_branch(p, value );

            case 6: // Coprocessor load/store and double register transfers
                arm_coprocessor_load_store(p, value);
            
            case 7: 
                //coprocessor data processing
                //coprocessor register transfers
                // software interrupt
                arm_coprocessor_others_swi(p, value);
        }
        // arm_constants.h pour les exceptions
    }
    //appel a la fonction arm_exception 
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
