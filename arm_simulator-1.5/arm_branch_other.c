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
    if( get_bits(ins, 27, 25)==5){
        //il s'agit d'une B ou BL
        if(get_bit(ins,24)==1){
            //il s'agit d'un BL
            p->register[14]=p->register[15];
         }
        uint32_t offset= (uint32_t )get_bits(ins,23,0);
        if(get_bit(offset,23)==1){
            offset = set_bits(offset, 31, 24, 0xFF)// adresse a rajouter a pc;
            offset=offset << 2;//multiplier par 4
        }
        p->register[15]=p->register[15]+offset;//pc<-pc+offset
        return 0;

    }

    else if (get_bits(ins,27,20)==18){
        //Il s'agit d'un BX ou BLX
        if(get_bits(ins,7,4)==1){
            //c'est un BX
            uint8_t n_registre= get_bits(ins,3,0);//n de registre code sur 4 bits
            uint32_t registre= p->reg[n_registre];//recuperer la valeur de ce registre
            p->register[15]=registre & 0xFFFFFFFE;//Page A4-20
            return 0;
        }
        /*else{PAS DE BLX
            //c'est BXL
            p->register[14]=p->register[15]; //LR<-adress
            p->register=p->register+get_bits(ins,3,0);

        }*/
    }
    return 1;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        return SOFTWARE_INTERRUPT;
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}
