/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>

int arm_branch(arm_core p, uint32_t ins) {
    //verification si c'est un branch immediate ou register
    if( get_bits(ins, 27, 25)==5){ //POURQUOI TU VERIFIE LA CONDIOTION ENCORE
        //il s'agit d'une B ou BL
        if(get_bit(ins,24)==1){
            //il s'agit d'un BL
            arm_write_register(p, 14, arm_read_register(p, 15));
        }
        uint32_t offset=get_bits(ins,23,0);
        if(get_bit(offset,23)==1){
            offset = set_bits(offset, 29, 24, 0x3F);
        }
        offset=logical_shift_left(offset , 2);
        arm_write_register(p, 14, arm_read_register(p, 15)+offset);//pc<-pc+offset 14 not pc
        return 0;

    }

    else if (get_bits(ins,27,20)==18){
        //Il s'agit d'un BX ou BLX
        uint8_t n_registre;
        uint32_t registre;
        if(get_bits(ins,7,4)==1){
            //c'est un BX
            n_registre= get_bits(ins,3,0);//n de registre code sur 4 bits
            registre= arm_read_register(p, n_registre);//recuperer la valeur de ce registre
            arm_write_register(p, 15, (registre & 0xFFFFFFFE));//Page A4-20
            return 0;
        }
        else if (get_bits(ins,7,4)==3){
            //c'est BXL
            n_registre= get_bits(ins,3,0);
            registre=arm_read_register(p, n_registre);
            arm_write_register(p, 14, registre); //LR<-adresse de Rm
            arm_write_register(p, 15, registre & 0xFFFFFFFE);
            return 0;
        }
        return UNDEFINED_INSTRUCTION;
    }else{
        return UNDEFINED_INSTRUCTION;
    }

}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        return SOFTWARE_INTERRUPT;
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
   /* if (get_bits(ins,27,23)==2 && get_bits(ins,21,20)==0){
        if(get_bit(ins,22)==1){
            p->reg->general_registers[get_bits(ins,15,12)] =  p->reg->spsr[p->reg->current_mode];
            return 0;
        }else{
            p->reg->general_registers[get_bits(ins,15,12)] = p->reg->cpsr;
            return 0;
        }
        return 1;
    } else {
        return UNDEFINED_INSTRUCTION;
    }*/
    return 0;
}