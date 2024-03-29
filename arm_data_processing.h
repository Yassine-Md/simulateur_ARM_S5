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
#ifndef __ARM_DATA_PROCESSING_H__
#define __ARM_DATA_PROCESSING_H__
#include <stdint.h>
#include "arm_core.h"

int arm_data_processing_shift(arm_core p, uint32_t ins);
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);
int rotate_right_with_extend(arm_core p,uint32_t rm,uint32_t *shifter_operand);
int add(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond );
int sub(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond );
int and(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond,int  c );
int orr(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c );
int eor(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c );
int rsb(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond);
int adc(arm_core p, uint32_t op_gauche, uint32_t op_droite,uint8_t rd,uint8_t set_cond, int carry_in);
int sbc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in );
int rsc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in );
int tst(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd, int c );
int teq(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,int c );
int cmp(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd);
int cmn(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd);
int mov(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c);
int mvn(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c);
int bic(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond);


#endif
