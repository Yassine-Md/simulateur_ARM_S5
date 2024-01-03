
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
#include "registers.h"
#include "arm_core.h"


int Arithmetic_shift_right_by_immediate(uint8_t shift_imm, uint32_t rm,uint32_t *shifter_operand){
    uint32_t value;
    if(shift_imm==0){
        if(get_bit(rm,31)==0){
            shifter_operand=0;
            return get_bit(rm,31);
        }else{
            value=0XFFFFFFFF;
            shifter_operand=&value;
            return get_bit(rm,31);
        }
    }else{
        value=asr(rm,shift_imm);
        shifter_operand=&value;
        return get_bit(rm,shift_imm-1);
    }
}


int Arithmetic_shift_right_by_register(uint32_t *shifter_operand,uint32_t rm,uint32_t rs,arm_core p){
    uint32_t value;
    if (get_bits(rs,7,0)==0){
        shifter_operand=&rm;
        return get_bit(arm_read_cpsr(p), 29); //return C flag
    }else if (get_bits(rs,7,0)<32){
        value= asr(rm, get_bits(rs,7,0));
        shifter_operand=&value;
        return get_bit(rm,get_bits(rs,7,0)-1);
    }else{
        if(get_bit(rm,31)==0){
            value=0;
            shifter_operand=&value;
            return get_bit(rm,31);
        }else{
            value=0XFFFFFFFF;
            shifter_operand=&value;
            return get_bit(rm,31);
        }
    }
}


int Rotate_right_by_immediate(arm_core p,uint8_t shift_imm,uint32_t rm,uint32_t *shifter_operand){
    uint32_t value;
    if(shift_imm==0){
        return rotate_right_with_extend(p,rm,shifter_operand);
    }else{
        value=ror(rm,shift_imm);
        shifter_operand=&value;
        return get_bit(rm,shift_imm -1);
    }
}


int rotate_right_by_register(uint32_t rs,uint32_t rm,uint32_t *shifter_operand,arm_core p){
    int shifter_carry_out;
    uint32_t value;
    //int c_flag= get_bit(arm_read_cpsr(p), 29);
    if (get_bits(rs,7,0)==0){
        value=rm;
        shifter_operand=&value;
        shifter_carry_out= get_bit(arm_read_cpsr(p), 29);
    }else if(get_bits(rs,4,0)==0){
        value=rm;
        shifter_operand=&value;
        shifter_carry_out=get_bit(rm,31);
    }else{
        value= ror(rm, get_bits(rs,4,0));
        shifter_operand=&value;
        shifter_carry_out= get_bit(rm, get_bits(rs,4,0)-1);
    }
    return shifter_carry_out;
}


int rotate_right_with_extend(arm_core p,uint32_t rm,uint32_t *shifter_operand){
    uint32_t value;
    int c_flag= get_bit(arm_read_cpsr(p), 29);
    value= logical_shift_left(c_flag , 31) | logical_shift_right(rm,1) ;
    shifter_operand=&value;
    int shifter_carry_out= get_bit(rm,0);
    return shifter_carry_out;
}


// Immediate
int immedate(arm_core p , uint8_t immed_8 ,uint8_t rotate_imm , uint32_t *shifter_operand ){
    uint32_t shifter_carry_out ;

    *shifter_operand= ror(immed_8 , rotate_imm*2 );

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
        shifter_operand = &Rm ;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); // reccuperer le C_flag
    }else{

        uint32_t x=logical_shift_left(Rm , shift_imm);
        shifter_operand =&x;
        shifter_carry_out = get_bit(Rm,32 - shift_imm); // 32 !! il faut 64 bits
    }
    return shifter_carry_out;
}

// Logical shift left by register
int logical_shift_left_by_register(arm_core p  , uint32_t Rm ,uint32_t *shifter_operand ,uint32_t Rs_value){ // Rs et Rm : sont les valeurs des registers
    uint32_t shifter_carry_out;
    uint32_t Rs_bit_7_0 = get_bits(Rs_value , 7 , 0);


    if (Rs_bit_7_0 == 0) {
        shifter_operand = &Rm;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); // recuperer C_flag
    }
    else if (Rs_bit_7_0 < 32) {
        uint32_t x= logical_shift_left(Rm,Rs_bit_7_0) ;//Rm Logical_Shift_Left Rs[7:0]
        shifter_operand=&x;
        shifter_carry_out =  get_bit(Rm , 32 - Rs_bit_7_0); //Rm[32 - Rs[7:0]]
    }
    else if (Rs_bit_7_0 == 32) {
        shifter_operand = 0;
        shifter_carry_out = get_bit(Rm,0);//Rm[0]
    }
    else {// Rs[7:0] > 32
        shifter_operand = 0 ;
        shifter_carry_out = 0 ;
    }
    return shifter_carry_out ;
}

// Logical shift right by immediate
int logical_shift_right_by_immediate(arm_core p , uint32_t shift_imm , uint32_t *shifter_operand , uint32_t Rm){
    uint32_t shifter_carry_out ;
    if (shift_imm == 0) {
        shifter_operand = 0;
        shifter_carry_out = get_bit(Rm , 31); //Rm[31]
    }

    else {// shift_imm > 0
        uint32_t x= logical_shift_right(Rm , shift_imm);//Rm Logical_Shift_Right shift_imm
        shifter_operand =&x;
        shifter_carry_out = get_bit(Rm ,shift_imm - 1);//Rm[shift_imm - 1]
    }
    return shifter_carry_out ;
}


// Logical shift right by register
int logical_shift_right_by_register(arm_core p , uint32_t Rs , uint32_t Rm ,uint32_t *shifter_operand){
    uint32_t shifter_carry_out ;
    uint32_t Rs_bit_7_0 = get_bits(Rs , 7 , 0);
    if( Rs_bit_7_0 == 0) {
        shifter_operand = &Rm ;
        shifter_carry_out = get_bit(arm_read_cpsr(p), 29); //reccupererC Flag
    }
    else if (Rs_bit_7_0 < 32) {
        uint32_t x= logical_shift_right(Rm , Rs_bit_7_0);//Rm Logical_Shift_Right Rs[7:0]
        shifter_operand=&x;
        shifter_carry_out = get_bit(Rm , Rs_bit_7_0 - 1);//Rm[Rs[7:0] - 1]
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


static int and(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond,int  c ){
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


static int eor(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c ){
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


static int sub(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
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


static int rsb(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond){
    uint32_t result = shifted_op2 - op1;
    arm_write_register(p, rd, shifted_op2);
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


static int add(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond ){
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


static int adc(arm_core p, uint32_t op_gauche, uint32_t op_droite,uint8_t rd,uint8_t set_cond, int carry_in) {
    int retour_add_partiel = add(p, op_gauche, op_droite, rd, set_cond);
    if ( retour_add_partiel == 0 ) {
        return add(p, arm_read_register(p, rd), carry_in, rd, set_cond);
    } else {
        return retour_add_partiel;
    }
}


static int sbc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in ){
    int retour_sub_partiel = sub(p, op1, shifted_op2, rd, set_cond);
    if ( retour_sub_partiel == 0 ) {
        return sub(p, arm_read_register(p, rd), ~(carry_in), rd, set_cond);
    } else {
        return retour_sub_partiel;
    }
}


static int rsc(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int carry_in ){
    int retour_rsb_partiel = rsb(p, op1, shifted_op2, rd, set_cond);
    if ( retour_rsb_partiel == 0 ) {
        return rsb(p, arm_read_register(p, rd), ~(carry_in), rd, set_cond);
    } else {
        return retour_rsb_partiel;
    }
}


// I bit 25 à revoir!!!
static int tst(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd, int c ){
    uint32_t result = op1 & shifted_op2;
    nzc_shifftercarry_update (p, result, c);
    return 0;
}


static int teq(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,int c ){
    uint32_t result = op1 ^ shifted_op2;
    nzc_shifftercarry_update (p, result, c);
    return 0;
}


static int cmp(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd){
    uint32_t result = op1 - shifted_op2;
    int C_flag = (result > op1);
    nzc_shifftercarry_update (p, result, C_flag);
    uint8_t V_flag = get_bit(op1,31) && !get_bit(result,31);
    arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));

    return 0;
}


static int cmn(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd){
    uint32_t result = op1 + shifted_op2;
    int C_flag = result < op1;
    nzc_shifftercarry_update (p, result, C_flag);
    uint8_t V_flag = !get_bit(op1,31) && get_bit(result,31);
    arm_write_cpsr(p,(arm_read_cpsr(p) & 0xEFFFFFFF)|(V_flag<<28));
    
    return 0;
}


static int orr(arm_core p, uint32_t op1, uint32_t shifted_op2,uint8_t rd,uint8_t set_cond, int c ){
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


static int mov(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c){

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


static int bic(arm_core p, uint32_t op1, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond){

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


static int mvn(arm_core p, uint32_t shifted_op2 ,uint8_t rd,uint8_t set_cond, int c){
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
