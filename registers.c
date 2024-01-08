#include "registers.h"
#include "util.h"
#include "arm_constants.h"
#include <stdlib.h>


// Structure pour représenter les données des registres
struct registers_data {
    uint32_t pc;
<<<<<<< HEAD
    uint32_t tab_r0_r7[8];   
    uint32_t tab_r8_r12[5][2];      
    uint32_t tab_r13_r14[2][6];      
=======
    uint32_t tab_r0_r7[8];
    uint32_t tab_r8_r12[5][2];
    uint32_t tab_r13_r14[2][6];
>>>>>>> de55d15050cbd9a6dedeb1d4014bbef4b506564d
    uint32_t cpsr;
    uint32_t spsr[5];         // SPSR pour chaque mode {FIQ , IRQ , SVC , ABT , UND }
    uint8_t current_mode;
};


registers registers_create() {
    registers r = (registers)malloc(sizeof(struct registers_data));
    if(r != NULL){
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
    if (r == NULL) {
        return UINT32_MAX; // ou une autre valeur appropriée en cas de pointeur NULL
    }

    switch (reg) {
        case 15:
            return r->pc;
        case 0 ... 7:
            return r->tab_r0_r7[reg];
        case 8 ... 12:
            return (mode == FIQ) ? r->tab_r8_r12[reg - 8][1] : r->tab_r8_r12[reg - 8][0];
        case 13:
        case 14:
            switch (mode) {
                case USR:
                case SYS:
                    return r->tab_r13_r14[reg-13][0] ;
                case FIQ:
                    return r->tab_r13_r14[reg-13][1] ;
                case IRQ:
                    return r->tab_r13_r14[reg-13][2] ;
                case SVC:
                    return r->tab_r13_r14[reg-13][3] ;
                case UND:
                    return r->tab_r13_r14[reg-13][4] ;
                case ABT:
                    return r->tab_r13_r14[reg-13][5] ;
            }
    }

    return UINT32_MAX; // en cas d'erreur
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


void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value) {
    if (r == NULL) {
        // Gurer le cas ou le pointeur vers les registres est NULL
        return;
    }
    if(reg == 15){
        r->pc = value ;
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

int mode_update(registers r){
    int mode_bits = get_bits(r->cpsr ,4 ,0);
    switch(mode_bits){
        case USR: return USR ;
        case FIQ: return FIQ ;
        case IRQ: return IRQ ;
        case SVC: return SVC ;
        case ABT: return ABT ;
<<<<<<< HEAD
        case UND: return UND ; 
=======
        case UND: return UND ;
>>>>>>> de55d15050cbd9a6dedeb1d4014bbef4b506564d
        case SYS: return SYS ;
        default :
            return -1;
    }
}

void registers_write_cpsr(registers r, uint32_t value) {
    if (r != NULL) {
        r->cpsr = value;
<<<<<<< HEAD
        //mettre a jour current_mode 
=======
        //mettre a jour current_mode ! masquage ...
>>>>>>> de55d15050cbd9a6dedeb1d4014bbef4b506564d
        r->current_mode = mode_update(r);
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


