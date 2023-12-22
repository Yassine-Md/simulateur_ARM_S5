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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"





/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    /*uint8_t opcode_shift = get_bit(ins,25);
    uint8_t set_cond = get_bit(ins, 20);
    uint8_t rn = get_bits(ins, 19, 16);
    uint8_t rd = get_bits(ins, 15, 12);
    uint8_t shift_type = get_bits(ins, 7, 6);
    uint8_t shift_imm = get_bits(ins, 11, 7);
    uint8_t rm = get_bits(ins, 3, 0);
    uint8_t opcode = get_bits(ins, 24, 21);

    uint32_t op1 = arm_read_register(p, rn);
    uint32_t shifted_op2 = 0;
    int c=carry_out();


    if (opcode_shift ==0) {
        // On extrait les valeurs de rn et rm
        uint32_t op2 = arm_read_register(p, rm);

        switch (shift_type) {
            case LSL:
                shifted_op2 = op2 << shift_imm;
                break;
            case LSR:
                shifted_op2 = op2 >> shift_imm;
                break;
            case ASR:
                shifted_op2 = asr(op2, shift_imm);
                break;
            case ROR:
                shifted_op2 = ror(op2, shift_imm);
                break;
            default:
                return UNDEFINED_INSTRUCTION;
        }
    }
    else{
        uint32_t op2 = rm;
        switch (shift_type) {
            case LSL:
                shifted_op2 = op2 << shift_imm;
                break;
            case LSR:
                shifted_op2 = op2 >> shift_imm;
                break;
            case ASR:
                shifted_op2 = asr(op2, shift_imm);
                break;
            case ROR:
                shifted_op2 = ror(op2, shift_imm);
                break;
            default:
                return UNDEFINED_INSTRUCTION;
            }
    }


     if(set_cond == 1){
            switch (opcode){
                case 0:
                    and(p,op1,shifted_op2,rd, set_cond,c);
                case 1:
                    eor(p,op1,shifted_op2, rd,set_cond);
                case 2:
                    sub(p,op1,shifted_op2,rd,set_cond);
                case 3:
                    rsb(p,op1,shifted_op2,rd,set_cond);
                case 4:
                    add(p,op1,shifted_op2,rd, set_cond);
                case 5:
                    adc(p,op1,shifted_op2,rd,set_cond);
                case 6:
                    sbc(p,op1,shifted_op2,rd,set_cond);
                case 7:
                    rsc(p,op1,shifted_op2,rd,set_cond);
                case 8:
                    tst(p,op1,shifted_op2, rd, set_cond,c);
                case 9:
                    teq(p,op1,shifted_op2, rd , set_cond);
                case 10:
                    cmp(p,op1,shifted_op2, rd , set_cond);
                case 11:
                    cmn(p,op1,shifted_op2, rd , set_cond);
                case 12:
                    orr(p,op1,shifted_op2, rd , set_cond,c);
                case 13:
                    mov(p,op1,shifted_op2, rd , set_cond,c);
                case 14:
                    bic(p,op1,shifted_op2, rd , set_cond);
                case 15:
                    mvn(p,op1,shifted_op2, rd , set_cond, c);
            }
        }*/
    return UNDEFINED_INSTRUCTION;
}



int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
   uint8_t SBO = get_bits(ins,15,12);
   uint8_t SBZ = get_bits(ins,11,8);
   uint8_t R =get_bit(ins,22);
   uint8_t I = get_bit(ins,25);
   uint8_t rotate_imm = get_bits(ins,11,8);
   uint8_t field_mask = get_bits(ins,19,16);
   uint8_t bits_0_7 = get_bits(ins,8,0);
   uint8_t operand;
   uint32_t mask ;
   /*-----------------------------------------------*/
    uint32_t user_mask  = 0xF0000000; 
    uint32_t priv_mask  = 0x0000000F;
    uint32_t state_mask = 0x00000020;
    /*-------------------------------------------------*/
   if (I == 0)
   {
       uint8_t Rm = get_bits(bits_0_7, 3, 0);
       uint8_t bit_0 = get_bits(bits_0_7, 7, 4);
        if (bit_0==0){
            operand = arm_read_register(p, Rm);
        }
        else{
            return UNDEFINED_INSTRUCTION;
        }
   }
    else{
        operand = ror(bits_0_7, rotate_imm);
    }
    uint32_t byte_mask=0x00000000;
    if(get_bit(field_mask, 0)==1){
        byte_mask |= 0x000000FF;
    }
    if(get_bit(field_mask, 0)==1){
        byte_mask |= 0x0000FF00;
    }
    if(get_bit(field_mask, 0)==1){
        byte_mask |= 0x00FF0000;
    }
    if(get_bit(field_mask, 0)==1){
        byte_mask |= 0xFF000000;
    }
    if (R==0){
        if (arm_in_a_privileged_mode(p)){
            if (operand &state_mask!=0){
                return UNDEFINED_INSTRUCTION;
            }
            else{
                uint32_t user_mask = 0xF8000000;
                uint32_t priv_mask = 0x0000000F;
                mask = byte_mask & (user_mask | priv_mask);
            }
        }
        else{
            mask = byte_mask & user_mask;
        }
        mask = byte_mask & user_mask;
        uint32_t cpsr = arm_read_cpsr(p);
        cpsr = (cpsr & !mask) | (operand & mask);
        arm_write_cpsr(p, cpsr);
    }
    else{
        if(arm_current_mode_has_spsr(p)){
            mask = byte_mask & (user_mask | priv_mask | state_mask);
            uint32_t spsr = arm_read_spsr(p);
            spsr = (spsr & !mask) | (operand & mask);
            arm_write_spsr(p, spsr);
        }
        else{
            return UNDEFINED_INSTRUCTION;
        }
    }
    return DATA_ABORT;
}
