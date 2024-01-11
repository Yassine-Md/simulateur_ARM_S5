//
// Created by amine on 19/12/23.
//
#include "arm_branch_other.h"
#include "memory.h"
#include "registers.h"
#include "arm_data_processing.h"
#include <assert.h>
#include "arm_constants.h"
#include "util.h"
typedef struct arm_core_data *arm_core;

int main(){
    //Test de Add
    memory mem= memory_create(1000);
    registers reg=registers_create();
    arm_core core= arm_create(reg,mem);
    registers_write(reg,2,USR,1);
    registers_write(reg,3,USR,2);
    add(core, registers_read(reg,2,USR),registers_read(reg,3,USR),4,1);
    assert(registers_read(reg,4,USR)==3);
    printf("test add OK\n");


    //Test de sub
    //Resultat positif
    sub(core,registers_read(reg,3,USR),registers_read(reg,2,USR),4,1);
    assert(registers_read(reg,4,USR)==1);
    //resulat negatif
    sub(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1);
    assert(registers_read(reg,4,USR)==-1);
    assert(get_bit(arm_read_cpsr(core),31)==1);//verifier l'activation du flag N

    printf("test sub Ok\n");


    //Test de and
    and(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,1);
    assert(registers_read(reg,4,USR)==0);
    printf("test and Ok\n");

    //Test de orr
    orr(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,1);
    assert(registers_read(reg,4,USR)==3);
    printf("test orr Ok\n");

    //Test de eor
    //test 1
    eor(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,0);
    assert(registers_read(reg,4,USR)==3);
    printf("test 1 eor Ok\n");
    //test 2
    registers_write(reg,2,USR,2);//modification du registre 2
    eor(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,0);
    assert(registers_read(reg,4,USR)==0);
    printf("test 2 eor Ok\n");


    //test rsb
    rsb(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1);
    assert(registers_read(reg,4,USR)==0);
    printf("test rsb Ok\n");


    //test adc
    //le carry est initialise a 1
    int c= get_bit(arm_read_cpsr(core),29);
    adc(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,c);
    assert(registers_read(reg,4,USR)==5);//verification si on a le bon resultat sur le registre destination
    assert(get_bit(registers_read(reg,4,USR),31)== get_bit(arm_read_cpsr(core),31));//N Flag = Rd[31]
    assert(get_bit(registers_read(reg,4,USR),30)==0);//Z Flag = if Rd == 0 then 1 else 0 dans ce cas rd!=0
    assert(get_bit(registers_read(reg,4,USR),29)==0);//C Flag = 0
    assert(get_bit(registers_read(reg,4,USR),28)==0);//V Flag = 0
    printf( "test adc Ok\n");

    //test sbc
    c= get_bit(arm_read_cpsr(core),29);
    sbc(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,c);
    assert(registers_read(reg,4,USR)==-1);
    assert(get_bit(arm_read_cpsr(core),31));//N flag active
    //Les restes flags restes les memes pas besoin de les changer
    printf("Test sbc Ok \n");


    //TEST RSC
    registers_write(reg,2,USR,10);
    registers_write(reg,3,USR,5);
    c= get_bit(arm_read_cpsr(core),29);
    rsc(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1,c);
    assert(registers_read(reg,4,USR)==-5);
    assert(get_bit(arm_read_cpsr(core),31));//N flag active
    printf("Test rsc Ok \n");

    //Test tst
    registers_write(reg,2,USR,0xFFFFFFFF);
    registers_write(reg,3,USR,0);
    tst(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1);
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag Z
    registers_write(reg,2,USR,0xFFFFFFFF);
    registers_write(reg,3,USR,0xFFFFFFFF);
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag N
    printf("Test Tst ok\n");

    //Test teq
    c= get_bit(arm_read_cpsr(core),29);
    teq(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,c);
    assert(!get_bit(arm_read_cpsr(core),31));//Tester le flag N
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag Z
    assert(get_bit(arm_read_cpsr(core),29)==c);//Tester le flag C
    assert(!get_bit(arm_read_cpsr(core),28));//Tester le flag V
    printf("Test teq ok\n");


    //Test cmp
    c= get_bit(arm_read_cpsr(core),29);
    cmp(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4);
    assert(!get_bit(arm_read_cpsr(core),31));//Tester le flag N
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag Z
    assert(!get_bit(arm_read_cpsr(core),29));//Tester le flag C
    assert(get_bit(arm_read_cpsr(core),28));//Tester le flag V
    printf("test cmp Ok\n");


    //Test cmn
    c= get_bit(arm_read_cpsr(core),29);
    //cas ou les 2 registres sont egaux
    cmn(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4);
    assert(get_bit(arm_read_cpsr(core),31));//Tester le flag N
    assert(!get_bit(arm_read_cpsr(core),30));//Tester le flag Z
    assert(get_bit(arm_read_cpsr(core),29));//Tester le flag C
    assert(!get_bit(arm_read_cpsr(core),28));//Tester le flag V
    printf("test cmn Ok\n");

    //test mov
    c=get_bit(arm_read_cpsr(core),29);
    mov(core,registers_read(reg,2,USR),4,1,c);
    assert(registers_read(reg,2,USR)==registers_read(reg,4,USR));
    assert(get_bit(arm_read_cpsr(core),31));//Tester le flag N
    printf("test mov Ok\n");

    //test mvn
    c=get_bit(arm_read_cpsr(core),29);
    mvn(core,registers_read(reg,2,USR),4,1,c);
    assert(registers_read(reg,2,USR) == ~(registers_read(reg,4,USR)));
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag z
    printf("test mvn ok\n");


    //test bic
    c=get_bit(arm_read_cpsr(core),29);
    bic(core,registers_read(reg,2,USR),registers_read(reg,3,USR),4,1);
    assert(registers_read(reg,4,USR)==0);
    assert(get_bit(arm_read_cpsr(core),30));//Tester le flag z
    printf("test bic OK\n");

    registers_destroy(reg);
    arm_destroy(core);
    return 0;
    

}
