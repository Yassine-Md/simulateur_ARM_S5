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



// Immediate 
int immedate(arm_core p , uint32_t *immed_8 ,uint32_t *rotate_imm , uint32_t *shifter_operand ){
    uint32_t shifter_carry_out ;

    shifter_operand == ror(immed_8 , rotate_imm*2 );
    if(rotate_imm == 0){
        shifter_carry_out = arm_read_cpsr(p); // a voir // reccuperer le C_flag
    }else{ // rotate_imm # 0
        shifter_carry_out = shifter_operand[31];
    }
    return shifter_carry_out ;
    
}

// register 
int registerr(arm_core p , uint32_t Rm , uint32_t *shifter_operand ){
    uint32_t shifter_carry_out ;
    shifter_operand = Rm ;
    shifter_carry_out = arm_read_cpsr(p) ; // recuuperer le C_flag
    return shifter_carry_out ;
}


// Logical shift left by immediate
 int logical_shift_left_by_immediate(arm_core p , uint32_t *shift_imm , uint32_t *Rm , uint32_t *shifter_operand){
    uint32_t shifter_carry_out ;
    if(shift_imm == 0){
        shifter_operand = Rm ;
        shifter_carry_out = arm_read_cpsr(p); // reccuperer le C_flag 
    }else{
        shifter_operand = logical_shift_left(Rm , shift_imm);
        shifter_carry_out = Rm[32 - shift_imm]; // 32 !! il faut 64 bits
    }
 }

// Logical shift left by register
int logical_shift_left_by_register(arm_core p , uint32_t *Rs , uint32_t *Rm ,uint32_t *shifter_operand ){ // Rs et Rm : sont les valeurs des registers 
    uint32_t shifter_carry_out;
    uint32_t Rs_bit_7_0 = get_bits(Rs_value , 7 , 0);


    if (Rs_bit_7_0 == 0) {
        shifter_operand = Rm
        shifter_carry_out = arm_read_cpsr(p) // reccup
    }
    else if (Rs_bit_7_0 < 32) {
        shifter_operand = logical_shift_left(Rm,Rs_bit_7_0) ;//Rm Logical_Shift_Left Rs[7:0]
        shifter_carry_out =  get_bit(Rm , 32 - Rs_bit_7_0); //Rm[32 - Rs[7:0]]
    }
    else if (get_bits(Rs_value , 7 , 0) == 32) {
        shifter_operand = 0;
        shifter_carry_out = get_bit(Rm,0);//Rm[0]
    }
    else {/* Rs[7:0] > 32 */
        shifter_operand = 0 ;
        shifter_carry_out = 0 ;
    }
    return shifter_carry_out ;
}

// Logical shift right by immediate
int logical_shift_right_by_immediate(arm_core p , uint32_t *shift_imm , uint32_t *shifter_operand , uint32_t *Rm){
    uint32_t shifter_carry_out ;
    if (shift_imm == 0) {
        shifter_operand = 0
        shifter_carry_out = get_bit(Rm , 31);//Rm[31]
    }
    
    else {/* shift_imm > 0 */
        shifter_operand = logical_shift_right(Rm , shift_imm);//Rm Logical_Shift_Right shift_imm
        shifter_carry_out = get_bit(Rm ,shift_imm - 1);//Rm[shift_imm - 1]
    }
    return shifter_carry_out ;
}


// Logical shift right by register
int logical_shift_right_by_register(arm_core p , uint32_t *Rs , uint32_t *Rm ,){
    uint32_t shifter_carry_out ;
    uint32_t Rs_bit_7_0 = get_bits(Rs , 7 , 0); 
    if( Rs_bit_7_0 == 0) {
        shifter_operand = Rm ;
        shifter_carry_out = arm_read_cpsr(p); //reccupererC Flag
    }
    else if (Rs_bit_7_0 < 32) {
        shifter_operand = logical_shift_right(Rm , Rs_bit_7_0);//Rm Logical_Shift_Right Rs[7:0]
        shifter_carry_out = get_bits(Rm , Rs_bit_7_0 - 1);//Rm[Rs[7:0] - 1]
    }
    else if (Rs_bit_7_0 == 32){
        shifter_operand = 0;
        shifter_carry_out = get_bit(Rm , 31);//Rm[31]
    }
    else{ /* Rs[7:0] > 32 */
        shifter_operand = 0 ;
        shifter_carry_out = 0;
    }
    return shifter_carry_out ;
}



static int and(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 & shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = op1==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
        
    }

    return 0;

}

static int add(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 + shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = result < op1;
        uint8_t V_flag = !get_bit(op1,31) && get_bit(result,31);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1)|(V_flag));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    return 0;
}


static int sub(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 - shifted_op2;
    arm_write_register(p, rd, result);


    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = result >= op1;
        uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1)|(V_flag));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }

    return 0;
}


static int orr(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 ^ shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    return 0;
}


static int mvn(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond){
    uint32_t result = ~shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }

    return 0;
}


static int mov(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond){
    uint32_t result = shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    return 0;
}



static int tst(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = op1 && shifted_op2;
    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    
    return 0;
}


static int rsb(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    uint32_t result = shifted_op2 - op1;
    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = result < op1;
        uint8_t V_flag = !get_bit(op1,31) && get_bit(result,31);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1)|(V_flag));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    
    return 0;
}




static int eor(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}







static int adc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}


static int sbc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}


static int rsc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}



static int teq(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}


static int cmp(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}


static int cmn(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
    return 0;
}




static int bic(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond){
    uint32_t result = ~shifted_op2;
    arm_write_register(p, rd, result);

    if(set_cond==1){
        uint8_t N_flag = get_bit(result,31);
        uint8_t Z_flag = result==0;
        uint8_t C_flag = get_bit(result,28);
        uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
        arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
    }
    
    
    return 0;
}



/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    uint8_t opcode_shift = get_bit(ins,25);
    uint8_t set_cond = get_bit(ins, 20);
    uint8_t rn = get_bits(ins, 19, 16);
    uint8_t rd = get_bits(ins, 15, 12);
    //uint8_t rs = get_bits(ins, 11, 8);
    uint8_t shift_type = get_bits(ins, 7, 6);
    uint8_t shift_imm = get_bits(ins, 11, 7);
    uint8_t rm = get_bits(ins, 3, 0);
    uint8_t opcode = get_bits(ins, 24, 21);

    uint32_t op1 = arm_read_register(p, rn);
    uint32_t shifted_op2 = 0;


    if (opcode_shift ==0) { //pour verifier si c'est une valeur passer par registre
        // On extrait les valeurs de rn et rm
        uint32_t op2 = arm_read_register(p, rm); //si oui on lu la valeur dans le registre

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
        uint32_t op2 = rm; //sinon on recupere la valeur immediate
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


     if(set_cond == 1){ //
            switch (opcode){
                case 0:
                    and(p,op1,shifted_op2,rd, set_cond);
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
                    tst(p,op1,shifted_op2, rd, set_cond);
                case 9:
                    teq(p,op1,shifted_op2, rd , set_cond);
                case 10:
                    cmp(p,op1,shifted_op2, rd , set_cond);
                case 11:
                    cmn(p,op1,shifted_op2, rd , set_cond);
                case 12:
                    orr(p,op1,shifted_op2, rd , set_cond);
                case 13:
                    mov(p,op1,shifted_op2, rd , set_cond);
                case 14:
                    bic(p,op1,shifted_op2, rd , set_cond);
                case 15:
                    mvn(p,op1,shifted_op2, rd , set_cond);
            }
        }
    return UNDEFINED_INSTRUCTION;
}



int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

