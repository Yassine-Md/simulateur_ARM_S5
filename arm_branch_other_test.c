//
// Created by amine on 19/12/23.
//
#include "arm_branch_other.h"
#include "registers.h"
#include "arm_core.h"
#include "memory.h"
#include "arm_constants.h"
#include <assert.h>

typedef struct arm_core_data *arm_core;

int main(){
    registers reg = registers_create();
    memory mem= memory_create(1000);
    arm_core core =arm_create(reg,mem);
    //test de B
    uint32_t b_instruction=0b11111010000000000000000010000000;
    uint32_t pc= arm_read_register(core,15);//valeur de pc avant le branch
    int res=arm_branch(core,b_instruction);
    if(res==0){
        assert(arm_read_register(core, 15)==pc+512+4);// 520=offset*4+pc+4
        printf("Test B ok\n");

    }

    //test BL
    uint32_t bl_instruction=0b11111011000000000000000010000000;
    uint32_t pc_avant_branch= arm_read_register(core,15);
    res= arm_branch(core,bl_instruction);
    //test si la valeur de lr change avec la valeur de pc
    // uint32_t lr=arm_read_register(core,15);
    if(res==0){
        assert(pc_avant_branch-4==arm_read_register(core,14));
        printf("Test BL ok\n");

    }

    //test BlX
    arm_write_register(core,2,128);
    uint32_t bx_instruction=0b11110001001000000000000000010010;
    pc_avant_branch= arm_read_register(core,15);
    res= arm_branch(core,bx_instruction);
    //on teste le changement de pc avec la valeur dans le registre 2

    return 0;
}