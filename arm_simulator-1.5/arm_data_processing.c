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
#include "registers.c"
#include "arm_core.c"




/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    uint8_t op_code = get_bits(ins, 24, 21);
    uint8_t set_cond = get_bit(ins, 20);
    uint8_t rn = get_bits(ins, 19, 16);
    uint8_t rd = get_bits(ins, 15, 12);
    //uint8_t rs = get_bits(ins, 11, 8);
    //uint8_t shift_type = get_bits(ins, 7, 6);
    //uint8_t shift_imm = get_bits(ins, 11, 7);
    //uint8_t rm = get_bits(ins, 3, 0);


    uint8_t opcode1 = get_bits(ins, 27, 26);

    uint32_t val_rn = p->reg->general_registers[rn];
    uint32_t val_rd = p->reg->general_registers[rd];
    

    if (opcode1==0b00){//ADDITION ou soustraction 
        if(op_code==4){// cas addition 
            uint32_t shift_op = get_bits(ins, 11, 0);
            val_rd= val_rn + shift_op;
            if((set_cond == 1) && (val_rd == p->reg->general_registers[15])){
                if (registers_current_mode_has_spsr(p->reg)){ //-----------------a demander au prof
                    p->reg->cpsr = arm_read_spsr(p);
                }else return UNDEFINED_INSTRUCTION;
            }
            else if (set_cond == 1) {
                uint8_t x = get_bit(val_rd,31);// recuperer N
                if(x){
                    p->reg->cpsr = set_bit(p->reg->cpsr, 31);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,31);
                }

                if(val_rn==0){ // recuperer Z                           a demander au prof
                    p->reg->cpsr = set_bit(p->reg->cpsr,30);
                }else   
                { p->reg->cpsr = clr_bit(p->reg->cpsr,30);}
                    uint8_t carry=val_rd< val_rn; //recuperer C
                if(carry){
                    p->reg->cpsr = set_bit(p->reg->cpsr,29);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,29);
                }
                
                uint8_t Vsign = !get_bit(val_rn,31) && get_bit(val_rd,31);  // recuperer V
                if (Vsign){
                    p->reg->cpsr = set_bit(p->reg->cpsr,28);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,28);
                }
        }}
        
        
        else if (op_code==2){
             uint32_t shift_op = get_bits(ins, 11, 0);
             val_rd= val_rn - shift_op; //opartion de soustraction 
             if((set_cond == 1) && (val_rd == p->reg->general_registers[15])){
                if (registers_current_mode_has_spsr(p->reg)){ //-----------------a demander au prof
                    p->reg->cpsr = arm_read_spsr(p);
                }else return UNDEFINED_INSTRUCTION;
            }
            else if (set_cond == 1) {
                uint8_t x = get_bit(val_rd,31); //recuperer N
                if(x){
                    p->reg->cpsr = set_bit(p->reg->cpsr, 31);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,31);
                }

                if(val_rn==0){ //recuperer Z                            a demander au prof
                    p->reg->cpsr = set_bit(p->reg->cpsr,30);
                }else   
                { p->reg->cpsr = clr_bit(p->reg->cpsr,30);}
                    uint8_t carry=val_rd> val_rn; //reuperer C
                if(carry){
                    p->reg->cpsr = set_bit(p->reg->cpsr,29);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,29);
                }
                
                uint8_t Vsign = get_bit(val_rn,31) && !get_bit(val_rd,31);//recuperer V
                if (Vsign){
                    p->reg->cpsr = set_bit(p->reg->cpsr,28);
                }else{
                    p->reg->cpsr = clr_bit(p->reg->cpsr,28);
                }
        }
		else if (op_code==0){
			uint32_t shift_op = get_bits(ins, 11, 0);
			val_rd= val_rn & (uint32_t) shift_op; //AND
			if((set_cond == 1) && (val_rd == p->reg->general_registers[15])){
				 if (registers_current_mode_has_spsr(p->reg)){ //-----------------a demander au prof
                    			p->reg->cpsr = arm_read_spsr(p);
                		} 
				 else return UNDEFINED_INSTRUCTION;
			} else if (set_cond == 1) {
				byte Neg=get_bit(val_rd,31);//recuperer N
				if(N){
                  			p->reg->cpsr = set_bit(p->reg->cpsr, 31);
               			 }else{
                  			  p->reg->cpsr = clr_bit(p->reg->cpsr,31);
             			 }

				byte Zero= val_rd == 0;
			if(Zero){ //recuperer Z                            a demander au prof
                    		p->reg->cpsr = set_bit(p->reg->cpsr,30);
               		 }else   
               		 { p->reg->cpsr = clr_bit(p->reg->cpsr,30);}
				byte C=//a voir demain;
				byte V =;
				
				
				
			}
		}
			


        }
	
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

