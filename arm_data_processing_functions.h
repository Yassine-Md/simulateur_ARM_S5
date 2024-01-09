
#ifndef __ARM_DATA_PROCESSING_FUNCTIONS_H
#define __ARM_DATA_PROCESSING_FUNCTIONS_H

#include <stdint.h>
#include "arm_core.h"

int carry(uint32_t ins, uint32_t *shifter_operand,arm_core p) ;
void nzc_shifftercarry_update(arm_core p, uint32_t result, int  c) ;
int Arithmetic_shift_right_by_immediate(uint8_t shift_imm, uint32_t rm,uint32_t *shifter_operand) ;
int Arithmetic_shift_right_by_register(uint32_t *shifter_operand,uint32_t rm,uint32_t rs,arm_core p) ;
int Rotate_right_by_immediate(arm_core p,uint8_t shift_imm,uint32_t rm,uint32_t *shifter_operand) ;
int rotate_right_by_register(uint32_t rs,uint32_t rm,uint32_t *shifter_operand,arm_core p) ;
int rotate_right_with_extend(arm_core p,uint32_t rm,uint32_t *shifter_operand) ;
int immedate(arm_core p , uint8_t immed_8 ,uint8_t rotate_imm , uint32_t *shifter_operand ) ;
int logical_shift_left_by_immediate(arm_core p , uint32_t shift_imm , uint32_t Rm , uint32_t *shifter_operand) ;
int logical_shift_left_by_register(arm_core p  , uint32_t Rm ,uint32_t *shifter_operand ,uint32_t Rs_value) ;
int logical_shift_right_by_immediate(arm_core p , uint32_t shift_imm , uint32_t *shifter_operand , uint32_t Rm) ;
int logical_shift_right_by_register(arm_core p , uint32_t Rs , uint32_t Rm ,uint32_t *shifter_operand) ;


#endif