
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
#include "util.h"
#include "debug.h"
#include "registers.h"
#include "arm_core.h"
#include "arm_data_processing_functions.h"


int and(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond,int  c ){
    uint32_t result = op1 & shifted_op2;
    arm_write_register(p, rd, result);
    if ((set_cond==1 ) && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        nzc_shifftercarry_update (p, result, c);
    }

    return 0;

}


int eor(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c ){
    uint32_t result = op1 ^ shifted_op2;
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
            return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        nzc_shifftercarry_update (p, result, c);
    }
    return 0;
}


int sub(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 - shifted_op2;
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        uint8_t C_flag = result > op1;
        nzc_shifftercarry_update (p, result, C_flag);
        uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    }

    return 0;
}


int rsb(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond){
    uint32_t result = shifted_op2 - op1;
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        int C_flag = !(get_bit(shifted_op2,31) && !get_bit(result,31));// shifter_carry_out
        nzc_shifftercarry_update (p, result, C_flag);
        uint8_t V_flag = get_bit(shifted_op2,31) && !get_bit(result,31);
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    }

    return 0;
}


 int add(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 + shifted_op2;
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        int C_flag = result < op1;
        nzc_shifftercarry_update (p, result, C_flag);
        uint8_t V_flag = !get_bit(op1,31) && get_bit(result,31);
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    }
    return 0;
}


int adc(arm_core p, uint32_t op_gauche, uint32_t op_droite,uint8_t rd,uint8_t set_cond, int carry_in) {
    int retour_add_partiel = add(p, op_gauche, op_droite, rd, set_cond);
    if ( retour_add_partiel == 0 ) {
        return add(p, arm_read_register(p, rd), carry_in, rd, set_cond);
    } else {
        return retour_add_partiel;
    }
}


int sbc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in ){
    uint32_t result = op1 - shifted_op2 - !(carry_in);
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))) {
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
            return UNDEFINED_INSTRUCTION;
        }
    }else if(set_cond==1){
        uint8_t C_flag = result > op1;
        nzc_shifftercarry_update (p, result, C_flag);
        uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    }
    return 0;
}


int rsc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in ){
    uint32_t result = shifted_op2 - op1 - !(carry_in);
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))) {
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
            return UNDEFINED_INSTRUCTION;
        }
    }else if(set_cond==1){
        uint8_t C_flag = result > op1;
        nzc_shifftercarry_update (p, result, C_flag);
        uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    }
    return 0;
}


// I bit 25 à revoir!!!
int tst(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd, int c ){
    uint32_t result = op1 & shifted_op2;
    nzc_shifftercarry_update (p, result, c);
    return 0;
}


int teq(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,int c ){
    uint32_t result = eor(p,op1,shifted_op2,rd,1,c);
    nzc_shifftercarry_update (p, result, c);
    return 0;
}


int cmp(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd){
    uint32_t result = op1 - shifted_op2;
    int C_flag = (result > op1);
    nzc_shifftercarry_update (p, result, C_flag);
    uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
    arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    return 0;
}


int cmn(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd){
    uint32_t result = op1 + shifted_op2;
    int C_flag = result < op1;
    nzc_shifftercarry_update (p, result, C_flag);
    uint8_t V_flag = !get_bit(op1,31) && get_bit(result,31);
    arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    
    return 0;
}


int orr(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c ){
    uint32_t result = op1 | shifted_op2;
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        nzc_shifftercarry_update (p, result, c);
    }
    return 0;
}


    int mov(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c){

    arm_write_register(p, rd, shifted_op2);
    if ((set_cond == 1) && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        nzc_shifftercarry_update (p, shifted_op2, c);
    }
    return 0;
}


int bic(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond){

    uint32_t result = op1 & ~(shifted_op2);
    arm_write_register(p, rd, result);
    if (set_cond == 1 && (arm_read_register(p,rd) == arm_read_register(p,15))){
        if (arm_current_mode_has_spsr(p)){
            arm_write_cpsr(p,arm_read_spsr(p));
        }else{
             return UNDEFINED_INSTRUCTION; 
        }
    } else if(set_cond==1){
        int C_flag = get_bit(result,28);
        nzc_shifftercarry_update (p, result, C_flag);
    }
    return 0;
}


 int mvn(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c){
    uint32_t shifted_op = ~(shifted_op2);
    return mov( p, shifted_op , rd, set_cond,  c);
}


        

int arm_data_processing_shift(arm_core p, uint32_t ins) {
    
    uint8_t n_op_g = get_bits(ins,19,16);//n operateur gauche
    uint32_t op_gauche= arm_read_register(p,n_op_g); //avoir la valuer de Rn
    uint32_t shifter_operand;
    int c=carry(ins,&shifter_operand,p);


    uint8_t n_r_res= get_bits(ins,15,12);//n registre destination

    
    uint8_t code_op= get_bits(ins,24,21);//op code
    uint8_t set_cond = get_bit(ins,20);
    switch (code_op){
        case 0:
            return and(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 1:
            return eor(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 2:
            return sub(p,op_gauche,shifter_operand,n_r_res,set_cond);
        case 3:
            return rsb(p,op_gauche,shifter_operand,n_r_res,set_cond);
        case 4:
            return add(p,op_gauche,shifter_operand,n_r_res,set_cond);
        case 5:
            return adc(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 6:
            return sbc(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 7:
            return rsc(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 8:
            return tst(p,op_gauche,shifter_operand,n_r_res,c);
        case 9:
            return teq(p,op_gauche,shifter_operand,n_r_res,c);
        case 10:
            return cmp(p,op_gauche,shifter_operand,n_r_res);
        case 11:
            return cmn(p,op_gauche,shifter_operand,n_r_res);
        case 12:
            return orr(p,op_gauche,shifter_operand,n_r_res,set_cond,c);
        case 13:
            return mov(p,shifter_operand,n_r_res,set_cond,c);
        case 14:
            return bic(p,op_gauche,shifter_operand,n_r_res,set_cond);
        case 15:
            return mvn(p,shifter_operand,n_r_res,set_cond,c);


    }
    return UNDEFINED_INSTRUCTION;
}




int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {

 
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
            if ((operand & state_mask) != 0){
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

