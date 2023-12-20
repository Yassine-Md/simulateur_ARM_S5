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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"





/*
P=1 et W=0 cela signifie que le registre de base reste inchangé après l'accès à la mémoire.
P=1 et W=1 cela signifie que le registre de base est mis à jour avec le résultat après l'accès à la mémoire
*/


/* A voir pour ajouter des conditions de verification
https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-instruction-set/memory-access-instructions/ldr-and-str--register-offset
*/



// elle renvoie quoi cette fonction
int arm_load_store(arm_core p, uint32_t ins) {
    // reccuperer COND 
    uint32_t cond = get_bits (ins , 31 , 28);
    //uint32_t load_store = get_bit(ins , 20); // 1 Load   0 Store
    //uint32_t B = get_bit(ins , 22); // Distinguishes between an unsigned byte (B==1) and a word (B==0) access.
    uint32_t Rn = get_bits(ins , 19 , 16); // Specifies the base register used by <addressing_mode>.
    //uint32_t Rd = get_bits(ins , 15 , 12); // Specifies the register whose contents are to be loaded or stored.
    uint32_t U = get_bit(ins , 23); 
    //uint32_t half_W = get_bits(ins , 27 , 25); 
    uint32_t P = get_bit(ins , 24);
    uint32_t W = get_bit(ins , 21);
    uint32_t bit_11_4 = get_bits (ins , 11 , 4);
    uint32_t I = get_bit(ins , 25);
    uint32_t offset_12 = get_bits(ins , 11 ,0);
    uint32_t Rm = get_bits(ins , 3 ,0);
    uint32_t shift_imm = get_bits(ins , 11 , 7);
    uint32_t shift = get_bits(ins,6,5);
    uint32_t bit_4 = get_bit(ins,4);
    uint32_t flags = get_bits(arm_read_cpsr(p), 31 , 28);
    uint32_t C_flag = get_bit (arm_read_cpsr(p) , 29);
    uint32_t bit_27_26 = get_bits (ins , 27 , 26);  // pour determiner si c'est un LDR/B STR/B | LDRH STRH
    //uint32_t always = 15;


/*  utiliser ces fonction pour profiter des traces 
int arm_read_word(arm_core p, uint32_t address, uint32_t *value) // return boolean
int arm_read_byte(arm_core p, uint32_t address, uint8_t *value)  // return boolean
int arm_write_byte(arm_core p, uint32_t address, uint8_t value)  // return boolean 
int arm_write_word(arm_core p, uint32_t address, uint32_t value) // return boolean 

void arm_write_register(arm_core p, uint8_t reg, uint32_t value) //return void modifie la valeur du registre Rreg = value
uint32_t arm_read_register(arm_core p, uint8_t reg) // return Rreg value 
*/

/*
if (load_store == 0){ // Store
    if(B == 0){ // Word
    	uint32_t val=arm_read_register(p,Rn)
     	uint32_t address=arm_read_register(p,Rd);
        arm_write_word(p,address,val);                  
    }else{ //Byte
        uint32_t val=arm_read_register(p,Rn)
     	uint32_t address=arm_read_register(p,Rd);
        arm_write_byte(p,address,(uint8_t)val);
    }
    
}else{ // Load
    if(B == 0){ // Word

    }else{ //Byte

    }

}


*/

    uint32_t chargedValue ;
    uint32_t address ;
    uint32_t value ;   // l'address memory de la valeur a stocker dans Rd
    uint32_t index ;

// stocker dans Rd la valeur de l'adresse qui se trouve dans Rn

    if (bit_27_26 == 1){ //  LDR/B STR/B 
        if (P == 0){
            if(W == 0){
                if(I == 0){
                    // immediate post-indexed
                    address = arm_read_register(p, Rn);
                    if(cond == flags){
                        if(U == 1){
                            value = arm_read_register(p, Rn) + offset_12 ;
                            arm_write_register(p, Rn, value);
                        }else{
                            value = arm_read_register(p, Rn) - offset_12 ;
                            arm_write_register(p, Rn, value);
                        }
                        if (load_store == 0){ // Store
                            if(B == 0){ // Word
                                //Rn 
                            }else{ //Byte
                                
                            }
                            
                        }else{ // Load
                            if(B == 0){ // Word
                                arm_read_word(p , value , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }else{ //Byte
                                arm_read_byte(p , value , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }

                        }

                    }else{ // si l'instruction ne verifie pas les condtion 
                        //Exception
                    }
                }else{  // I == 1
                    if(bit_11_4 == 0){
                        //register post indexed
                        address = arm_read_register(p, Rn);
                        if(cond == flags){
                            if(U == 1){
                                value = arm_read_register(p, Rn) + arm_read_register(p, Rm) ;
                                arm_write_register(p, Rn, value);
                            }else{
                                value = arm_read_register(p, Rn) - arm_read_register(p, Rm);
                                arm_write_register(p, Rn, value);
                            }
                            if (load_store == 0){ // Store
                                if(B == 0){ // Word
                                                                
                                }else{ //Byte
                                        
                                }
                                
                            }else{ // Load
                                if(B == 0){ // Word
                                    arm_read_word(p , value , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }else{ //Byte
                                    arm_read_byte(p , value , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }

                            }
                        }else{ // si l'instruction ne verifie pas les condtion 
                            //Exception
                        }
                    }
                    else if(bit_4 == 0){
                        // scaled register post-indexed
                        address = arm_read_register(p , Rn);
                        switch (shift){
                            case 0:
                                index = logical_shift_left (arm_read_register(p , Rm) , shift_imm);
                                break;
                            case 1:
                                if(shift_imm == 0){
                                    index = 0;
                                }else{
                                    index = logical_shift_right(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            case 2:
                                if(shift_imm == 0){
                                    if(get_bit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else{
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3:
                                if(shift_imm == 0){
                                    index = logical_shift_left(C_flag , 31) | logical_shift_right(arm_read_register(p , Rm) , 1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                        }
                        if(cond == flags){
                            if(U == 1){
                                value = arm_read_register(p, Rn) + index;
                                arm_write_register(p, Rn, value);
                            }else{
                                value = arm_read_register(p, Rn) - index;
                                arm_write_register(p, Rn, value);
                            }
                            if (load_store == 0){ // Store
                                if(B == 0){ // Word
                                                                
                                }else{ //Byte
                                        
                                }
                                
                            }else{ // Load
                                if(B == 0){ // Word
                                    arm_read_word(p , value , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }else{ //Byte
                                    arm_read_byte(p , value , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }

                            }
                        }else{ // si l'instruction ne verifie pas les condtion 
                            //Exception
                        }
                        
                    }else{
                        // undefined instruction 
                    }
                }
            }else{ // W == 1
                // undefined instruction  p459 privilege mode strt ldrt ...
            }

        }else{ // P == 1
            if (W == 0){
                if(I == 0){
                    // immediate offset
                    if (U == 1){
                        address = arm_read_register(p , Rn) + offset_12 ;
                    }else{
                        address= arm_read_register(p , Rn) - offset_12 ;
                    }
                    if (load_store == 0){ // Store
                        if(B == 0){ // Word
                                                        
                        }else{ //Byte
                                
                        }
                        
                    }else{ // Load
                        if(B == 0){ // Word
                            arm_read_word(p , address , &chargedValue);
                            arm_write_register(p, Rd, chargedValue);
                        }else{ //Byte
                            arm_read_byte(p , address , &chargedValue);
                            arm_write_register(p, Rd, chargedValue);
                        }

                    }
                // load ou store (byte or word) manipulation 

                }else{ // I == 1
                    if(bit_11_4 == 0){
                            
                        //register offset 
                        if (U == 1){
                            address  = arm_read_register(p , Rn) + arm_read_register(p , Rm) ;
                        }else{
                            address  = arm_read_register(p , Rn) - arm_read_register(p , Rm) ;
                        }
                        if (load_store == 0){ // Store
                            if(B == 0){ // Word
                                                            
                            }else{ //Byte
                                    
                            }
                        }else{ // Load
                            if(B == 0){ // Word
                                arm_read_word(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }else{ //Byte
                                arm_read_byte(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }

                        }
                        
                    }
                    else if(bit_4 == 0){
                        // scaled register offset 
                        switch (shift){
                            case 0:
                                index = logical_shift_left(arm_read_register(p , Rm) , shift_imm) ; 
                            case 1:
                                if (shift_imm == 0){
                                    index = 0 ;
                                }else{
                                    logical_shift_right(arm_read_register(p , Rm) , shift_imm) ;
                                }
                                break ;
                            case 2:
                                if(shift_imm == 0){
                                    if(get_bit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else {
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3: 
                                if(shift_imm == 0){
                                    index  = logical_shift_left(C_flag, 31) | logical_shift_right(arm_read_register(p , Rm) ,1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            default :
                                break; // return undefined 
                        }
                        if(U == 1){
                            address = arm_read_register(p , Rn) + index ;
                        }else{
                            address = arm_read_register(p , Rn) - index ;
                        }
                        if (load_store == 0){ // Store
                            if(B == 0){ // Word
                                                            
                            }else{ //Byte
                                    
                            }
                        
                        }else{ // Load
                            if(B == 0){ // Word
                                arm_read_word(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }else{ //Byte
                                arm_read_byte(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }

                        }
                    }else{
                        // undefined instruction 
                    }

                }
            }else{ // W == 1
                if(I == 0){
                //immediate pre-indexed
                    if(U == 1){
                        address = arm_read_register(p , Rn) + offset_12 ;
                    }else{
                        address = arm_read_register(p , Rn) - offset_12 ;
                    }
                    // si cond == au flags ZNCV de CPSR
                    if (cond == flags){
                        arm_write_register(p , Rn , address);

                        if (load_store == 0){ // Store
                            if(B == 0){ // Word
                                                            
                            }else{ //Byte
                                    
                            }
                        
                        }else{ // Load
                            if(B == 0){ // Word
                                arm_read_word(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }else{ //Byte
                                arm_read_byte(p , address , &chargedValue);
                                arm_write_register(p, Rd, chargedValue);
                            }

                        }
                    }else{ // si l'instruction ne verifie pas les condtion 
                        //Exception
                    }

                }else{ // I == 1
                    if(bit_11_4 == 0){
                        //register pre-indexed
                        if(U == 1){
                            address = arm_read_register(p , Rn) + arm_read_register(p , Rm) ;
                        }else{
                            address = arm_read_register(p , Rn) - arm_read_register(p , Rm) ;
                        }
                        if (cond == flags){
                            arm_write_register(p , Rn , address);

                            if (load_store == 0){ // Store
                                if(B == 0){ // Word
                                                                
                                }else{ //Byte
                                        
                                }
                            
                            }else{ // Load
                                if(B == 0){ // Word
                                    arm_read_word(p , address , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }else{ //Byte
                                    arm_read_byte(p , address , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }

                            }
                        }else{// si l'instruction ne verifie pas les condtion 
                            //Exception}
                        }
                    else if(bit_4 == 0){
                        //scaled register pre-indexed
                        switch (shift){
                            case 0:
                                index = logical_shift_left(arm_read_register(p , Rm) , shift_imm) ; 
                            case 1:
                                if (shift_imm == 0){
                                    index = 0 ;
                                }else{
                                    logical_shift_right(arm_read_register(p , Rm) , shift_imm) ;
                                }
                                break ;
                            case 2:
                                if(shift_imm == 0){
                                    if(get_bit(arm_read_register(p , Rm) , 31) == 1){
                                        index = 0xFFFFFFFF ;
                                    }else{
                                        index = 0 ;
                                    }
                                }else {
                                    index = asr(arm_read_register(p , Rm) , shift_imm);
                                }
                                break ;
                            case 3: 
                                if(shift_imm == 0){
                                    index  = logical_shift_left(C_flag, 31) | logical_shift_right(arm_read_register(p , Rm) ,1);
                                }else{
                                    index = ror(arm_read_register(p , Rm) , shift_imm);
                                }
                                break;
                            default :
                                break; // return undefined 
                        }
                        if(U == 1){
                            address = arm_read_register(p , Rn) + index ;
                        }else{
                            address = arm_read_register(p , Rn) - index ;
                        }
                        if(cond == flags){
                            arm_write_register(p , Rn , address);
                            if (load_store == 0){ // Store
                                if(B == 0){ // Word
                                                                
                                }else{ //Byte
                                        
                                }
                            
                            }else{ // Load
                                if(B == 0){ // Word
                                    arm_read_word(p , address , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }else{ //Byte
                                    arm_read_byte(p , address , &chargedValue);
                                    arm_write_register(p, Rd, chargedValue);
                                }

                            }
                        }else{// si l'instruction ne verifie pas les condtion 
                            //Exception
                        }
                        
                    }
                    else{
                        //undefined 
                    }  
                }
            }

        }

    }else{ //LDRH STRH

    }

    return UNDEFINED_INSTRUCTION;
}

//*c'est l'exepetion DATA_ABORT
//pour le load/store normal on va utiliser les commande LDM et STM */
int arm_load_store_multiple(arm_core p, uint32_t ins) {
    //uint8_t cond = get_bits(ins,31,28);
    uint8_t P = get_bit(ins,24);//indicateur du mode d'adressage 1:mode post_increment(adresses sont incrémentées après le transfert)
    // 2:mode pre_increment(adresses sont décrémentées avant le transfert)
    uint8_t U = get_bit(ins,23);//bit U spécifie la direction de l'incrémentation/decrémentation des adresses. S'il est à 1, les adresses sont 
    //incrémentées. S'il est à 0, les adresses sont décrémentées.
    //uint8_t W = get_bit(ins,21);//si W==1 le registre Rn sera mis a jour 0 si il reste inchagé
    //bit 22 il doit etre 0 
    uint8_t Rn = get_bits(ins,19,16);
    uint16_t reg_list=get_bits(ins,15,0);
    if (get_bit(ins, 20 )==0){
        //STM
        uint32_t address=arm_read_register(p,(int) Rn);
        for (int i=0;i<15;i++){
            if (get_bit(reg_list,i)==1){
                if (P==1){
                    if (U==1){
                        address=address+4;
                    }
                    else{
                    address=address-4;
                    }
                }
                uint32_t val = arm_read_register(p,i)
                arm_write_word(p,address,val);
                if(P==0){ 
                    if (U==1){
                        address=address+4;
                    }
                    else{
                        address=address-4;
                    }
                }
            }
        }

    }
    else{
        //LDM
        uint32_t address=arm_read_register(p,(int) Rn);
        for (int i=0 ;i<15;i++){
            if (get_bit(reg_list,i)==1){
                if (P==1){
                    if (U==1){
                        address=address+4;
                    }
                    else{
                    address=address-4;
                    }
                }
                uint32_t value;
                arm_read_word(p,address,&value);
                arm_write_register(p,i,value);
                if(P==0){ 
                    if (U==1){
                        address=address+4;
                    }
                    else{
                        address=address-4;
                    }
                }
            }
        }
        //cette condition verifie si le R15 (pc) est passé comme un registre dans le champs registre list si c'est le cas 
        //elle essei d'ajjuster sa valeur a partir de la memoire 
        /*uint32_t valeur =arm_read_register(p,15);
        if (valeur==1){
            uint32_t val;
            arm_read_word(p,address,&val);
            val
            //p->reg->registers[15]=val & 0xFFFFFFFE;  a verifier 

        }*/ //toutes cette condition est a verifier 
    }
    return DATA_ABORT;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
