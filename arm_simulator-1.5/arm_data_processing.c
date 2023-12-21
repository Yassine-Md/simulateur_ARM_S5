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
    return UNDEFINED_INSTRUCTION;
}

