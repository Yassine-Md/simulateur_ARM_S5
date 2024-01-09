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

#define ABORT_MODE 0b10111
#define IRQ_MODE   0b10010
#define UND_MODE   0b11011
#define FIQ_MODE   0b10001


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
        data_abort(p);
    }

    if(exception == INTERRUPT){
        unterrupt(p);
    }

    if(exception == UNDEFINED_INSTRUCTION){
        undefined_unstruction(p);
    }

    if(exception == PREFETCH_ABORT){
        fetch_abort(p);
    }

    if(exception == FAST_INTERRUPT){
        fast_interrupt_request(p);
    }

    // terminer le traitement de undefined instruction apres interruption


     return exception;


}


// function to set the CPSR bits for exception handling
void set_exception_cpsr(arm_core p, uint8_t mode , uint8_t bit6 ) {
    // SPSR_mode = CPSR    sauvgarder CPSR dans SPSR du mode correspondant 
    arm_write_spsr(p,arm_read_cpsr(p));
    // CPSR[4:0] = mode
    arm_write_cpsr(p, (arm_read_cpsr(p) & ~(0x0000001f)) | mode);
    // CPSR[5] = 0 (Execute in ARM state)
    arm_write_cpsr(p, clr_bit(arm_read_cpsr(p), 5));
    // CPSR[6] = bit6
    if (bit6) {
        arm_write_cpsr(p, set_bit(arm_read_cpsr(p), 6));
    } else {
        // unchanged
    }
    // CPSR[7] = 1 (Disable normal interrupts)
    arm_write_cpsr(p, set_bit(arm_read_cpsr(p), 7));
    // CPSR[8] = 1 (Disable Imprecise Data Aborts)
    arm_write_cpsr(p, set_bit(arm_read_cpsr(p), 8));
    // CPSR[9] = CP15_reg1_EEbit (Endianness on exception entry)
    arm_write_cpsr(p, clr_bit(arm_read_cpsr(p), 9));
}


void data_abort(arm_core p){
    //R14_abt = address of the aborted instruction + 8
    arm_write_register(p,14,(arm_read_register(p,15) -4) + 8);
    //SPSR_abt = CPSR
    
    set_exception_cpsr(p , 0b10111, 0 );
    arm_write_register(p, 15, 0x00000010); 
}

void unterrupt(arm_core p){
    // R14_irq = address of next instruction to be executed + 4
    arm_write_register(p,14,(arm_read_register(p,15) -4) + 4);
    //SPSR_irq = CPSR
    
    set_exception_cpsr( p, 0b10010 , 0 );
    arm_write_register(p, 15, 0x00000018);
}

void undefined_unstruction(arm_core p){
    //R14_und = address of next instruction after the Undefined instruction
    arm_write_register(p,14,arm_read_register(p,15));
    //SPSR_und  = CPSR
    
    set_exception_cpsr( p, 0b11011 , 0 );
    arm_write_register(p, 15, 0x00000004);
}


void fetch_abort(arm_core p){
    // R14_abt= address of the aborted instruction + 4
    uint32_t aborted_instruction_address = arm_read_register(p,15) - 4;
    uint32_t value = aborted_instruction_address + 4 ; 
    arm_write_register(p , 14 , value);
    //SPSR_abt = CPSR
    
    set_exception_cpsr( p, 0b10111 , 0 );
    arm_write_register(p,15,0x0000000C);
}

void fast_interrupt_request(arm_core p){
    // R14_fiq = address of next instruction to be executed + 4
    uint32_t next_instruction_address = arm_read_register(p,15) ;
    uint32_t value = next_instruction_address + 4;
    arm_write_register(p,14,value);
    
    set_exception_cpsr( p, 0b10001 , 1 );
    arm_write_register(p,15,0x0000001C);
}

