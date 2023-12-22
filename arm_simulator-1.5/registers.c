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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>


// Structure pour représenter les données des registres
struct registers_data {
    uint32_t tab_r0_r7[8];   
    uint32_t tab_r8_r12[5][2];      
    uint32_t tab_r13_r14[2][6];      
    uint32_t cpsr;
    uint32_t spsr[5];         // SPSR pour chaque mode {FIQ , IRQ , SVC , ABT , UND }
    uint8_t current_mode;
};


registers registers_create() {
    registers r = (registers)malloc(sizeof(struct registers_data));
    if(r != NULL){

        // pas besoin d'initialiser les valeurs du tableau parce qu''il y a un reset au tout debut de lancement de arm_simulator

        // Initialiser le mode par defaut (SVC)  : pour faire passer les testes (USR ne passe pas le teste )
        r->current_mode = SVC;
    }
    return r ;

}

void registers_destroy(registers r) {
    if (r != NULL) {
        free(r);
    }
}

uint8_t registers_get_mode(registers r) {
    return r->current_mode ;
}


static int registers_mode_has_spsr(registers r, uint8_t mode) {
    switch (mode) {
        case SVC:
        case FIQ:
        case ABT:
        case UND:
        case IRQ:
            return 1; // true, le mode a un SPSR associe
        default:
            return 0; // false, le mode n'a pas de SPSR associe
    }
}

int registers_current_mode_has_spsr(registers r) {
    return registers_mode_has_spsr(r, registers_get_mode(r));
}


int registers_in_a_privileged_mode(registers r) {
    if (r != NULL){
        if (r->current_mode == USR || r->current_mode == SYS){
            return 0; // false, pas dans un mode privilegie
        }
        return 1; // true, dans un mode privilegie
    }
    return 0; // false, en cas de pointeur NULL
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode) {
        if(reg>=0 && reg<8){
            return r->tab_r0_r7[reg];
        }
        
        if (reg >= 8 && reg <= 12) {
            if(mode == FIQ){
                return r->tab_r8_r12[reg - 8][1];
            }else{
                return r->tab_r8_r12[reg - 8][0];
            }
        
        }
        if(reg == 13 || reg == 14){
            switch (mode){
                case USR:
                case SYS:
                    return r->tab_r13_r14[reg-13][0] ;
                case FIQ:
                    return r->tab_r13_r14[reg-13][1] ;
                case IRQ:
                    return r->tab_r13_r14[reg-13][2];
                case SVC:
                    return r->tab_r13_r14[reg-13][3] ;
                case UND:
                    return r->tab_r13_r14[reg-13][4] ;
                case ABT:
                    return r->tab_r13_r14[reg-13][5] ;
            }
        }
        return UINT32_MAX ; // en cas d'erreur 
}

uint32_t registers_read_cpsr(registers r) {
    if (r != NULL){
        return r->cpsr;
    }
    return UINT32_MAX ;
}

uint32_t registers_read_spsr(registers r, uint8_t mode) {
    if (r != NULL){ 
        if (mode == USR || mode == SYS){
            return UINT32_MAX;
        }else {
            switch (mode){
                case SVC:
                    return r->spsr[2];
                case FIQ:
                    return r->spsr[0];
                case ABT:
                    return r->spsr[3];
                case UND:
                    return r->spsr[4];
                case IRQ:
                    return r->spsr[1];
            }
       }
    }
    return UINT32_MAX ;
}


void registers_write(struct registers_data* r, uint8_t reg, uint8_t mode, uint32_t value) {
    if (r == NULL) {
        // Gérer le cas où le pointeur vers les registres est NULL
        return;
    }

    if (reg >= 0 && reg < 8) {
        r->tab_r0_r7[reg] = value;
    } else if (reg >= 8 && reg <= 12) {
        if (mode == FIQ) {
            r->tab_r8_r12[reg - 8][1] = value;
        } else {
            r->tab_r8_r12[reg - 8][0] = value;
        }
    } else if (reg == 13 || reg == 14) {
        switch (mode) {
            case USR:
            case SYS:
                r->tab_r13_r14[reg - 13][0] = value;
                break;
            case FIQ:
                r->tab_r13_r14[reg - 13][1] = value;
                break;
            case IRQ:
                r->tab_r13_r14[reg - 13][2] = value;
                break;
            case SVC:
                r->tab_r13_r14[reg - 13][3] = value;
                break;
            case UND:
                r->tab_r13_r14[reg - 13][4] = value;
                break;
            case ABT:
                r->tab_r13_r14[reg - 13][5] = value;
                break;
        }
    }
}

void registers_write_cpsr(registers r, uint32_t value) {
    if (r != NULL) {
        r->cpsr = value;
        //mettre a jour current_mode ! masquage ...
    }
}


void registers_write_spsr(registers r, uint8_t mode, uint32_t value) {
    if (r != NULL && registers_mode_has_spsr(r, mode)) {
        switch (mode) {
            case FIQ:
                r->spsr[0] = value;
                break;
            case IRQ:
                r->spsr[1] = value;
                break;
            case SVC:
                r->spsr[2] = value;
                break;
            case ABT:
                r->spsr[3] = value;
                break;
            case UND:
                r->spsr[4] = value;
                break;
            default:
                break;
        }
    }
}
