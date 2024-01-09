
#include "arm_data_processing_functions.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
#include "registers.h"
#include "arm_core.h"

int Arithmetic_shift_right_by_immediate(uint8_t shift_imm, uint32_t rm,uint32_t *shifter_operand){
    if(shift_imm==0){
        if(get_bit(rm,31)==0){
            *shifter_operand=0;
            return get_bit(rm,31);
        }else{
            *shifter_operand=0XFFFFFFFF;
            return get_bit(rm,31);
        }
    }else{
        *shifter_operand=asr(rm,shift_imm);;
        return get_bit(rm,shift_imm-1);
    }
}


int Arithmetic_shift_right_by_register(uint32_t *shifter_operand,uint32_t rm,uint32_t rs,arm_core p){
    if (get_bits(rs,7,0)==0){
        *shifter_operand=rm;
        return get_bit(arm_read_cpsr(p), 29); //return C flag
    }else if (get_bits(rs,7,0)<32){
        *shifter_operand=asr(rm, get_bits(rs,7,0));;
        return get_bit(rm,get_bits(rs,7,0)-1);
    }else{
        if(get_bit(rm,31)==0){
            *shifter_operand=0;
            return get_bit(rm,31);
        }else{
            *shifter_operand=0XFFFFFFFF;
            return get_bit(rm,31);
        }
    }
}


int Rotate_right_by_immediate(arm_core p,uint8_t shift_imm,uint32_t rm,uint32_t *shifter_operand){
    if(shift_imm==0){
        return rotate_right_with_extend(p,rm,shifter_operand);
    }else{
        *shifter_operand=ror(rm,shift_imm);
        return get_bit(rm,shift_imm -1);
    }
}


int rotate_right_by_register(uint32_t rs,uint32_t rm,uint32_t *shifter_operand,arm_core p){
    int shifter_carry_out;
    //int c_flag= get_bit(arm_read_cpsr(p), 29);
    if (get_bits(rs,7,0)==0){
        *shifter_operand=rm;
        shifter_carry_out= get_bit(arm_read_cpsr(p), 29);
    }else if(get_bits(rs,4,0)==0){
        *shifter_operand=rm;
        shifter_carry_out=get_bit(rm,31);
    }else{
        *shifter_operand= ror(rm, get_bits(rs,4,0));
        shifter_carry_out= get_bit(rm, get_bits(rs,4,0)-1);
    }
    return shifter_carry_out;
}


int rotate_right_with_extend(arm_core p,uint32_t rm,uint32_t *shifter_operand){
    int c_flag= get_bit(arm_read_cpsr(p), 29);
    *shifter_operand= logical_shift_left(c_flag , 31) | logical_shift_right(rm,1) ;
    int shifter_carry_out= get_bit(rm,0);
    return shifter_carry_out;
}


// Immediate
int immedate(arm_core p , uint8_t immed_8 ,uint8_t rotate_imm , uint32_t *shifter_operand ){
    uint32_t shifter_carry_out ;
    
    *shifter_operand = ror(immed_8 , rotate_imm*2 );
    if(rotate_imm == 0){
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); // reccuperer le C_flag
    }else{ // rotate_imm # 0
        shifter_carry_out = get_bit(*shifter_operand,31);
    }
    return shifter_carry_out ;
}


// Logical shift left by immediate
int logical_shift_left_by_immediate(arm_core p , uint32_t shift_imm , uint32_t Rm , uint32_t *shifter_operand){
    uint32_t shifter_carry_out ;
    if(shift_imm == 0){
        *shifter_operand = Rm ;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); // reccuperer le C_flag
    }else{

        *shifter_operand=logical_shift_left(Rm , shift_imm);
        shifter_carry_out = get_bit(Rm,32 - shift_imm); // 32 !! il faut 64 bits
    }
    return shifter_carry_out;
}

// Logical shift left by register
int logical_shift_left_by_register(arm_core p  , uint32_t Rm ,uint32_t *shifter_operand ,uint32_t Rs_value){ // Rs et Rm : sont les valeurs des registers
    uint32_t shifter_carry_out;
    uint32_t Rs_bit_7_0 = get_bits(Rs_value , 7 , 0);


    if (Rs_bit_7_0 == 0) {
        *shifter_operand = Rm;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); // recuperer C_flag
    }
    else if (Rs_bit_7_0 < 32) {
        uint32_t x= logical_shift_left(Rm,Rs_bit_7_0) ;//Rm Logical_Shift_Left Rs[7:0]
        *shifter_operand=x;
        shifter_carry_out =  get_bit(Rm , 32 - Rs_bit_7_0); //Rm[32 - Rs[7:0]]
    }
    else if (Rs_bit_7_0 == 32) {
        *shifter_operand = 0;
        shifter_carry_out = get_bit(Rm,0);//Rm[0]
    }
    else {// Rs[7:0] > 32
        *shifter_operand = 0 ;
        shifter_carry_out = 0 ;
    }
    return shifter_carry_out ;
}

// Logical shift right by immediate
int logical_shift_right_by_immediate(arm_core p , uint32_t shift_imm , uint32_t *shifter_operand , uint32_t Rm){
    uint32_t shifter_carry_out ;
    if (shift_imm == 0) {
        *shifter_operand = 0;
        shifter_carry_out = get_bit(Rm , 31); //Rm[31]
    }

    else {// shift_imm > 0
        uint32_t x= logical_shift_right(Rm , shift_imm);//Rm Logical_Shift_Right shift_imm
        *shifter_operand =x;
        shifter_carry_out = get_bit(Rm ,shift_imm - 1);//Rm[shift_imm - 1]
    }
    return shifter_carry_out ;
}


// Logical shift right by register
int logical_shift_right_by_register(arm_core p , uint32_t Rs , uint32_t Rm ,uint32_t *shifter_operand){
    uint32_t shifter_carry_out ;
    uint32_t Rs_bit_7_0 = get_bits(Rs , 7 , 0);
    if( Rs_bit_7_0 == 0) {
        *shifter_operand = Rm ;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); //reccupererC Flag
    }
    else if (Rs_bit_7_0 < 32) {
        *shifter_operand= logical_shift_right(Rm , Rs_bit_7_0);//Rm Logical_Shift_Right Rs[7:0]
        shifter_carry_out = get_bit(Rm , Rs_bit_7_0 - 1);//Rm[Rs[7:0] - 1]
    }
    else if (Rs_bit_7_0 == 32){
        *shifter_operand = 0;
        shifter_carry_out = get_bit(Rm , 31);//Rm[31]
    }
    else{ /* Rs[7:0] > 32 */
        *shifter_operand = 0 ;
        shifter_carry_out = 0;
    }
    return shifter_carry_out ;
}


int carry(uint32_t ins, uint32_t *shifter_operand,arm_core p){

    uint8_t imm;
    uint8_t rotate;
    uint8_t rm;
    uint8_t rs;
    uint32_t shift_amount;
    int c;

    if(get_bit(ins,25)){
        //Data-processing operands - Immediate
        imm = get_bits(ins,7,0);
        rotate = get_bits(ins,11,8);

        c=immedate(p,imm,rotate, shifter_operand);
    }else{

        rm= get_bits(ins,3,0);
        uint32_t value_rm=arm_read_register(p,rm);//registre a decaler avant de executer l'instruction
        
        if(get_bit(ins,4)==1) {
            //data processing register shift [2]
            rs= get_bits(ins,11,8);
            shift_amount = arm_read_register(p,rs); //registre pour savoir le decalage a faire
        }else {
            //data processing immediate shift
            shift_amount = get_bits(ins, 11, 7);
        }


        if(get_bits(ins,6,4 )==0){
            c=logical_shift_left_by_immediate(p,shift_amount, value_rm, shifter_operand);
        }
        if(get_bits(ins,7,4)){

            c=logical_shift_left_by_register(p,value_rm,shifter_operand,shift_amount);
        }
        if(get_bits(ins,6,4)==2){
            c=logical_shift_right_by_immediate(p,shift_amount,shifter_operand,value_rm);
        }
        if(get_bits(ins,7,4)==3){
            c=logical_shift_right_by_register(p,shift_amount,value_rm,shifter_operand);
        }
        if(get_bits(ins,6,4)==4){
            c=Arithmetic_shift_right_by_immediate(shift_amount,value_rm,shifter_operand);
        }
        if(get_bits(ins,7,4)==5){
            c=Arithmetic_shift_right_by_register(shifter_operand,value_rm,shift_amount,p);
        }
        if(get_bits(ins,6,4)==6){
            c=Rotate_right_by_immediate(p,shift_amount,value_rm,shifter_operand);
        }
        if(get_bits(ins,11,4)==6){
            c=rotate_right_with_extend(p,value_rm,shifter_operand) ;
        }


    }
    return c;
}



void nzc_shifftercarry_update (arm_core p, uint32_t result, int  c){
    uint8_t N_flag = get_bit(result,31);
    uint8_t Z_flag = (result==0);
    int C_flag = c;// shifter_carry_out
    uint8_t Tout_flag = ((N_flag<<3)|(Z_flag<<2)|(C_flag<<1));
    arm_write_cpsr(p,(arm_read_cpsr(p) & 0x1FFFFFFF)|(Tout_flag<<28));
}