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
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"

// Not supported below ARMv6, should read as 0
#define CP15_reg1_EEbit 0
#define Exception_bit_9 (CP15_reg1_EEbit << 9)

int arm_exception(arm_core p, uint8_t exception) {
    uint32_t cpsr = 0x1d3 | Exception_bit_9;
    /* As there is no operating system in our simulator, we handle
     * software interrupts here :
     * - 0x123456 is the end of the simulation
     * - other opcodes can be used for any custom behavior,
     *   such as my_putchar given as an example
     */
    if (exception == SOFTWARE_INTERRUPT) {
        uint32_t value;
        uint32_t address = arm_read_register(p, 15);
        address -= 8;
        uint32_t instruction;
        arm_read_word(p, address, &instruction);
        instruction = instruction & 0xFFFFFF;
        switch (instruction) {
        case 0x123456:
            return END_SIMULATION;
        case 0x000001:
            value = arm_read_register(p, 0);
            putchar(value);
            return 0;
        }
    }
    /* Aside from SWI, we only support RESET initially */
    /* Semantics of reset interrupt (ARM manual A2-18) */
    if (exception == RESET) {
        arm_write_cpsr(p, cpsr);
        arm_write_register(p, 15, 0);
    }
   

    if(exception == DATA_ABORT){
        //R14_abt = address of the aborted instruction + 8
        arm_write_register(p,14,(arm_read_register(p,15) -4) + 8);
        //SPSR_abt = CPSR
        arm_write_spsr(p,arm_read_cpsr(p));
        //CPSR[4:0] = 0b10111 Enter Abort mode 
        arm_write_cpsr(p,(arm_read_cpsr(p) & ~(0x0000000F)) | 0b10111);
        //CPSR[5] = 0  Execute in ARM state 
        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),5));
        //CPSR[7] = 1  Disable normal interrupts 
        arm_write_cpsr(p,set_bit(arm_read_cpsr(p),7));
        //CPSR[8] = 1 /* Disable Imprecise Data Aborts (v6 only) 
        arm_write_cpsr(p,set_bit(arm_read_cpsr(p),8));
        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),9));
        arm_write_register(p, 15, 0);
    }

    if(exception == INTERRUPT){
        //R14_abt = address of the aborted instruction + 8
        arm_write_register(p,14,(arm_read_register(p,15) -4) + 4);
        //SPSR_abt = CPSR
        arm_write_spsr(p,arm_read_cpsr(p));
        //CPSR[4:0] = 0b10111 Enter Abort mode 
        arm_write_cpsr(p,(arm_read_cpsr(p) & ~(0x0000000F)) | 0b10010);
        //CPSR[5] = 0  Execute in ARM state 
        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),5));
        //CPSR[7] = 1  Disable normal interrupts 
        arm_write_cpsr(p,set_bit(arm_read_cpsr(p),7));
        //CPSR[8] = 1 /* Disable Imprecise Data Aborts (v6 only) 
        arm_write_cpsr(p,set_bit(arm_read_cpsr(p),8));
        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),9));
        arm_write_register(p, 15, 0x00000018);
    }

    if(exception == UNDEFINED_INSTRUCTION){
        //R14_abt = address of the aborted instruction + 8
        arm_write_register(p,14,arm_read_register(p,15));
        //SPSR_abt = CPSR
        arm_write_spsr(p,arm_read_cpsr(p));
        //CPSR[4:0] = 0b10111 Enter Abort mode 
        arm_write_cpsr(p,(arm_read_cpsr(p) & ~(0x0000000F)) |  0b11011);
        //CPSR[5] = 0  Execute in ARM state 
        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),5));
        //CPSR[7] = 1  Disable normal interrupts 
        arm_write_cpsr(p,set_bit(arm_read_cpsr(p),7));

        arm_write_cpsr(p,clr_bit(arm_read_cpsr(p),9));
        arm_write_register(p, 15, 0x00000004);
    }



     return exception;


}
