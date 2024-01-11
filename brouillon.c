// Structure pour representer une paire (mode, register_value)
struct mode_register_pair {
    uint8_t mode;
    uint32_t register_value;
};

// Structure pour representer les modes d'un registre general
struct registers_mode {
    struct mode_register_pair modes[5];  // 5 modes
};

// Structure pour representer les donnees des registres
struct registers_data {
    struct registers_mode* general_registers[16];  
    uint32_t cpsr;
    uint32_t spsr[5];  // SPSR pour chaque mode {FIQ , IRQ , SVC , ABT , UND }
    uint8_t current_mode;
};

//---------------------------------------------------
                //nouvelle structure
//---------------------------------------------------

// Structure pour representer les valeurs
struct values {
    uint32_t* valeurs; // un tableau de taille variable
};

// Structure pour representer les modes
struct modes {
    struct values* modes[7]; // 5 modes ou chaque mode pointe vers un tableau de valeurs de taille comprise entre 1 et 5
};

// Structure pour representer les données des registres
struct registers_data {
    struct modes* general_registers;  // tableau de registers au chaque register pointe vers un tableau de mode 
    uint32_t cpsr;
    uint32_t spsr[5];  // SPSR pour chaque mode {FIQ , IRQ , SVC , ABT , UND }
    uint8_t current_mode;
};


// Fonction : registers_create
// Modifie : Alloue et initialise une structure de registres. Initialise les registres generaux, le CPSR, le mode par défaut (SVC) et les SPSR.
// Return : Pointeur vers la structure de registres nouvellement cree.
registers registers_create() {
    registers r = (registers)malloc(sizeof(struct registers_data));

    // Initialisation des registres généraux et des SPSR pour chaque mode
    for (int i = 0; i < 16; i++) {
        r->general_registers->modes[i] = (struct values*)malloc(sizeof(struct values));

        // Allouer dynamiquement le tableau de valeurs en fonction du numéro de registre
        if (i <= 7 || i == 15) {
            r->general_registers->modes[i]->valeurs = (uint32_t*)malloc(sizeof(uint32_t));
        } else if (i <= 12 && i >= 8) {
            r->general_registers->modes[i]->valeurs = (uint32_t*)malloc(2 * sizeof(uint32_t));
        } else { // R13 et R14
            r->general_registers->modes[i]->valeurs = (uint32_t*)malloc(6 * sizeof(uint32_t));
        }
        
        // Ne pas initialiser les valeurs ici il y a le reset !!
    }

    // Initialisation du CPSR et du mode par défaut (SVC)
    r->cpsr = 0;
    r->current_mode = SVC;

    return r;
}

void registers_destroy(registers r) {
    // Liberer la memoire allouee pour les registres generaux et les SPSR pour chaque mode
    for (int i = 0; i < 16; i++) {
        free(r->general_registers->modes[i]->valeurs);
        free(r->general_registers->modes[i]);
    }

    // Liberer la mémoire de la structure des registres
    free(r);
}




uint32_t registers_read(registers r, uint8_t reg, uint8_t mode) {
    if(r != NULL){
        if(reg < 16 && reg >= 0 ){
            return r->general_registers[reg];
        }
    }
    return UINT32_MAX;  // Valeur speciale indiquant une erreur (definie dans la bib stdint)
}






// Structure pour représenter les données des registres
struct registers_data {
    uint32_t tab_r0_r7[8];   
    uint32_t tab_r8_r12[5][2];      
    uint32_t tab_r13_r14[2][6];      
    uint32_t cpsr;
    uint32_t spsr[5];         // SPSR pour chaque mode {FIQ , IRQ , SVC , ABT , UND }
    uint8_t current_mode;
};


registers registers_create() {
    registers r = (registers)malloc(sizeof(struct registers_data));
    if(r != NULL){

        // pas besoin d'initialiser les valeurs du tableau parce qu''il y a un reset au tout debut de lancement de arm_simulator

        // Initialiser le mode par defaut (SVC)  : pour faire passer les testes (USR ne passe pas le teste )
        r->current_mode = SVC;
    }
    return r ;

}

void registers_destroy(registers r) {
    if (r != NULL) {
        free(r);
    }
}

uint8_t registers_get_mode(registers r) {
    return r->current_mode ;
}


static int registers_mode_has_spsr(registers r, uint8_t mode) {
    switch (mode) {
        case SVC:
        case FIQ:
        case ABT:
        case UND:
        case IRQ:
            return 1; // true, le mode a un SPSR associe
        default:
            return 0; // false, le mode n'a pas de SPSR associe
    }
}

int registers_current_mode_has_spsr(registers r) {
    return registers_mode_has_spsr(r, registers_get_mode(r));
}


int registers_in_a_privileged_mode(registers r) {
    if (r != NULL){
        if (r->current_mode == USR || r->current_mode == SYS){
            return 0; // false, pas dans un mode privilegie
        }
        return 1; // true, dans un mode privilegie
    }
    return 0; // false, en cas de pointeur NULL
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode) {
        if(reg>=0 && reg<8){
            return tab_r0_r7[reg]
        }
        
        if (reg >= 8 && reg <= 12) {
            if(mode == IFQ){
                return r->tab_r8_r12[reg - 8][1];
            }else{
                return r->tab_r8_r12[reg - 8][0];
            }
        
        }
        if(reg == 13 || reg == 14){
            switch (mode){
                case USR:
                case SYS:
                    return r->tab_r13_r14[reg-13][0] ;
                case FIQ:
                    return r->tab_r13_r14[reg-13][1] ;
                case IRQ:
                    return r->tab_r13_r14[reg-13][2];
                case SVC:
                    return r->tab_r13_r14[reg-13][3] ;
                case UND:
                    return r->tab_r13_r14[reg-13][4] ;
                case ABT:
                    return r->tab_r13_r14[reg-13][5] ;
            }
        }
    
}

uint32_t registers_read_cpsr(registers r) {
    if (r != NULL){
        return r->cpsr;
    }
    return UINT32_MAX ;
}

uint32_t registers_read_spsr(registers r, uint8_t mode) {
    if (r != NULL){ 
        if (mode == USR || mode == SYS){
            return UINT32_MAX;
        }else {
            switch (mode){
                case SVC:
                    return r->spsr[2];
                case FIQ:
                    return r->spsr[0];
                case ABT:
                    return r->spsr[3];
                case UND:
                    return r->spsr[4];
                case IRQ:
                    return r->spsr[1];
            }
       }
    }
    return UINT32_MAX ;
}


void registers_write(struct registers_data* r, uint8_t reg, uint8_t mode, uint32_t value) {
    if (r == NULL) {
        // Gérer le cas où le pointeur vers les registres est NULL
        return;
    }

    if (reg >= 0 && reg < 8) {
        r->tab_r0_r7[reg] = value;
    } else if (reg >= 8 && reg <= 12) {
        if (mode == FIQ) {
            r->tab_r8_r12[reg - 8][1] = value;
        } else {
            r->tab_r8_r12[reg - 8][0] = value;
        }
    } else if (reg == 13 || reg == 14) {
        switch (mode) {
            case USR:
            case SYS:
                r->tab_r13_r14[reg - 13][0] = value;
                break;
            case FIQ:
                r->tab_r13_r14[reg - 13][1] = value;
                break;
            case IRQ:
                r->tab_r13_r14[reg - 13][2] = value;
                break;
            case SVC:
                r->tab_r13_r14[reg - 13][3] = value;
                break;
            case UND:
                r->tab_r13_r14[reg - 13][4] = value;
                break;
            case ABT:
                r->tab_r13_r14[reg - 13][5] = value;
                break;
        }
    }
}

void registers_write_cpsr(registers r, uint32_t value) {
    if (r != NULL) {
        r->cpsr = value;
        //mettre a jour current_mode ! masquage ...
    }
}


void registers_write_spsr(registers r, uint8_t mode, uint32_t value) {
    if (r != NULL && registers_mode_has_spsr(r, mode)) {
        switch (mode) {
            case FIQ:
                r->spsr[0] = value;
                break;
            case IRQ:
                r->spsr[1] = value;
                break;
            case SVC:
                r->spsr[2] = value;
                break;
            case ABT:
                r->spsr[3] = value;
                break;
            case UND:
                r->spsr[4] = value;
                break;
            default:
                break;
        }
    }
}

















#include <stdio.h>
#include <stdlib.h>
#include "arm_load_store.h"
#include "arm_core.h"
#include "memory.h"
#include "registers.h"
#include "util.h"


void compare(uint32_t a, uint32_t b) {
    if (a == b){
    	printf("SUCCESS ( %i == %i )",a,b);
    } else {
    	printf("FAILURE ( %i != %i )",a,b);
    }
}

int main(void){
	int returnValue;
	uint8_t reg,registerValueByte,memoryValueByte ;
    uint16_t registerValueHalf , memoryValueHalf ;
	uint32_t value, ins,address,registerValue,memoryValue;
	memory mem = memory_create(512);
    registers reg = registers_create(); 
    if ((mem == NULL) || (reg == NULL)) {
        fprintf(stderr, "Error when creating simulated [memory | register]\n");
        exit(1);
    }

	arm_core p = arm_create(mem , reg);

	


	//STRB R0,R2, #+0 w/ P=1 & W=0
	ins = 0b11110101110000000010000000000000 ;

	returnValue = arm_load_store(p,ins);

	address = arm_read_register(p,2);
	registerValueByte = arm_read_register(p,0);
	arm_read_byte(p,address, &memoryValueByte);
	printf("Ecriture de %i sur 8 bits a l'address %i : ",registerValueByte,address);
	compare(registerValueByte,memoryValueByte);
	printf(" <returnValue %i> \n",returnValue);

	//STR R5,R2,#+2 w/ P=1 & W=0
	ins = 0b11110101100000100101000000000010;

	returnValue = arm_load_store(p,ins);

	address = arm_read_usr_register(p,2);
	registerValue = arm_read_usr_register(p,5);
	arm_read_word(p,address, &memoryValue);
	printf("Ecriture de %i sur 32 bits a l'address %i: ",registerValue,address);
	compare(registerValue,memoryValue);
	printf(" <returnValue %i> \n",returnValue);

	//LDR R12,R2,#+0 w/ P=1 & W=0
	ins = 0b11110101100100101100000000000000;

	returnValue = arm_load_store(p,ins);
	
	address = arm_read_usr_register(p,2);
	registerValue = arm_read_usr_register(p,12);
	printf("Transfere dans R%i du contenu a l'address %i: ",12,address);
	arm_read_word(p,address,&memoryValue);
	compare(registerValue,memoryValue);
	printf(" <returnValue %i> \n",returnValue);


	return 0;
}













#define INTERRUPT -2
#define UNDEFINED_INSTRUCTION -1
#define BX_BLX 1
#define LDR_STR 2
#define LDRH_STRH 3
#define LDM_STM 4
#define B_BL 5
#define miscellaneous 6
#define DATA_PROCESSING 7



int type_instructions(uint32_t value){
    uint8_t bit_21_20 = get_bits(value , 21 , 20) ;
    uint8_t bit_24_23 = get_bits(value , 24 , 23) ;
    uint8_t bit_4 = get_bit(value, 4);
    uint8_t bit_20 = get_bit(value , 20);
    uint8_t bit_7 = get_bit(value , 7);
    //reccuperer les bits de 25 a 27 de value pour avoir la categorie de l'instruction
    uint8_t cat_inst = get_bits(value , 27 , 25) ;  // categorie d'instruction
    if(ConditionPassed(p , value)){ // si les conditons sont verifier 
        switch(cat_inst){
            case 0:
                if (get_bits(value,27,20)==18){
                    return BX_BLX;  //BX AND BLX
                }
                if(get_bits(value,7,4)==0b1011){
                    return LDRH_STRH ;       // load store HALF
                }
                if(bit_24_23 == 2 && bit_20 == 0){
                    // Miscellaneous instructions:
                    return miscellaneous ;  // pour mrs
                } else if(bit_4 == 1 && bit_7 == 1){
                    return UNDEFINED_INSTRUCTION ;// undefine on va pas traiter les extra load store et les multiplies
                } else if (bit_4 == 1 || bit_4 == 0 ){
                    return arm_data_processing_shift(p, value);
                }else{
                    return UNDEFINED_INSTRUCTION ;
                }
            case 1:
                if ((bit_24_23 == 2)&& (bit_21_20 == 0)){
                    // Undifined instruction
                    return UNDEFINED_INSTRUCTION ;
                }else{
                    //move immediate to status register  (mettre à jour le registre d'état avec une valeur immédiate)
                    //data prcessing immediate (effectuer des opérations arithmétiques ou logiques sur des données immédiates)
                    return DATA_PROCESSING;
                }
            case 2: // Load/store immediate offset
                return LDR_STR;

            case 3: // Load/store register offset
                if(bit_4 == 0){
                    return LDR_STR;
                }else{
                    if(get_bits(value,24,20) == 0b11111){
                        // architecturally undefined
                        return UNDEFINED_INSTRUCTION;
                    }
                    else{
                        // media instruction 
                        return -1 ;
                    }   
                }
            case 4: // load store multiple
                return LDM_STM;

            case 5: // branche with link

                return B_BL;

            case 6: // Coprocessor load/store and double register transfers
                return -1;
            
            case 7: 
                if(get_bit(value , 24) == 0){
                    //coprocessor data processing
                    //coprocessor register transfers
                    return -1;
                }else{
                    // software interrupt
                    return INTERRUPT ;
                }
        }
    }else{ // les conditions ne sont pas verifier
        return INTERRUPT ;
    }
}


int main(int argc , char *argv[]){
    printf("type d'instruction est %d \n " ,type_instructions ((int)argv[1]));
    return 0 ;
}































/*

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.414: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.414: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.414: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.415: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.415: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.415: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.416: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.416: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.416: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.417: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.417: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.417: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.418: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.418: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.418: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-WARNING **: 16:51:38.419: A tag named 'invalid-char-style' is already in the tag table.

(gitg:11604): GtkSourceView-CRITICAL **: 16:51:38.419: gtk_source_style_apply: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_tag_set_priority: assertion 'GTK_IS_TEXT_TAG (tag)' failed

(gitg:11604): Gtk-CRITICAL **: 16:51:38.419: gtk_text_buffer_apply_tag: assertion 'GTK_IS_TEXT_TAG (tag)' failed
                                                                                                                   

*/
gdb-multiarch -ex "file Examples/example1" -ex "target remote localhost:44000" -ex "set endian big" -ex "load" 




#!/bin/bash

if [ $# -ne 1 ]
then
	echo "[Fichier a tester] ERREUR. 
        nom de fichier .s a executer !"
else
	#filename=$(basename $1)
	filename=$1

	# Recupere le numero de la derniere ligne : swi 0x123456
	lastLine=$(grep -n 'swi 0x123456' $filename'.s' | awk -F ":" '{print $1}')	
	if [ -f "arm_simulator" ] #verifier si l'executable est present
	then
		> log.out
		./arm_simulator --gdb-port 44000 &
		gdb-multiarch -ex "set pagination off" \
		-ex "file $filename" \
		-ex "target remote localhost:44000" \
        -ex "set endian big" \
		-ex "load" \
		-ex "b $lastLine" \ 
		-ex "c" \
		-ex "set logging file log.out" \
		-ex "set logging on" \
		-ex "info reg" \
		-ex "set logging off" \
		-ex "continue" \
		-ex "quit" &> /dev/null

		> log_gdb.out
		gdb-multiarch -ex "set pagination off" \
		-ex "file $filename" \
		-ex "target sim" \
		-ex "load" \
		-ex "b 1" \
		-ex "run" \
		-ex "advance $lastLine" \
		-ex "set logging file log_gdb.out" \
		-ex "set logging on" \
		-ex "info reg" \
		-ex "set logging off" \
		-ex "continue" \
		-ex "quit" &> /dev/null

		
		echo "---------------------{GDB}----------------------"
		echo "$(<log_gdb.out)"
		echo "------------------------------------------------"
		echo "---------------------{RESULTAT}----------------------"
		echo "$(<log.out)"
		echo "------------------------------------------------"
		echo "--------------------{DIFF}--------------------"
		echo "$(diff log.out log_gdb.out)"
		echo "------------------------------------------------"

	else
		echo "[Erreur] l'executable du simulateur n'est pas present dans le dossier courant."
	fi
fi
*/


uint32_t registers_read(registers r, uint8_t reg, uint8_t mode) {
    if (r == NULL) {
        return UINT32_MAX; // ou une autre valeur appropriée en cas de pointeur NULL
    }

    switch (reg) {
        case 15:
            return r->pc;
        case 0 ... 7:
            return r->tab_r0_r7[reg];
        case 8 ... 12:
            return (mode == FIQ) ? r->tab_r8_r12[reg - 8][1] : r->tab_r8_r12[reg - 8][0];
        case 13:
        case 14:
            switch (mode) {
                case USR:
                case SYS:
                    return r->tab_r13_r14[reg-13][0] ;
                case FIQ:
                    return r->tab_r13_r14[reg-13][1] ;
                case IRQ:
                    return r->tab_r13_r14[reg-13][2] ;
                case SVC:
                    return r->tab_r13_r14[reg-13][3] ;
                case UND:
                    return r->tab_r13_r14[reg-13][4] ;
                case ABT:
                    return r->tab_r13_r14[reg-13][5] ;
            }
    }

    return UINT32_MAX; // en cas d'erreur
}






















#include "util.h"
#include "arm_instruction.h"
#include <stdlib.h>
#include <stdio.h>



#define INTERRUPTION -2
#define UNDEFINED_INSTRUCTIONS -1
#define BX_BLX 1
#define LDR_STR 2
#define LDRH_STRH 3
#define LDM_STM 4
#define B_BL 5
#define miscellaneous 6
#define DATA_PROCESSING 7


int type_instructions(uint32_t value){
    uint8_t bit_21_20 = get_bits(value , 21 , 20) ;
    uint8_t bit_24_23 = get_bits(value , 24 , 23) ;
    uint8_t bit_4 = get_bit(value, 4);
    uint8_t bit_20 = get_bit(value , 20);
    uint8_t bit_7 = get_bit(value , 7);
    //reccuperer les bits de 25 a 27 de value pour avoir la categorie de l'instruction
    uint8_t cat_inst = get_bits(value , 27 , 25) ;  // categorie d'instruction
    if(1){ // si les conditons sont verifier 
        switch(cat_inst){
            case 0:
                if (get_bits(value,27,20)==18){
                    return BX_BLX;  //BX AND BLX
                }
                if(get_bits(value,7,4)==0b1011){
                    return LDRH_STRH ;       // load store HALF
                }
                if(bit_24_23 == 2 && bit_20 == 0){
                    // Miscellaneous instructions:
                    return miscellaneous ;  // pour mrs
                } else if(bit_4 == 1 && bit_7 == 1){
                    return UNDEFINED_INSTRUCTIONS ;// undefine on va pas traiter les extra load store et les multiplies
                } else if (bit_4 == 1 || bit_4 == 0 ){
                    return DATA_PROCESSING;
                }else{
                    return UNDEFINED_INSTRUCTIONS ;
                }
            case 1:
                if ((bit_24_23 == 2)&& (bit_21_20 == 0)){
                    // Undifined instruction
                    return UNDEFINED_INSTRUCTIONS ;
                }else{
                    //move immediate to status register  (mettre à jour le registre d'état avec une valeur immédiate)
                    //data prcessing immediate (effectuer des opérations arithmétiques ou logiques sur des données immédiates)
                    return DATA_PROCESSING;
                }
            case 2: // Load/store immediate offset
                return LDR_STR;

            case 3: // Load/store register offset
                if(bit_4 == 0){
                    return LDR_STR;
                }else{
                    if(get_bits(value,24,20) == 0b11111){
                        // architecturally undefined
                        return UNDEFINED_INSTRUCTIONS;
                    }
                    else{
                        // media instruction 
                        return -1 ;
                    }   
                }
            case 4: // load store multiple
                return LDM_STM;

            case 5: // branche with link

                return B_BL;

            case 6: // Coprocessor load/store and double register transfers
                return -1;
            
            case 7: 
                if(get_bit(value , 24) == 0){
                    //coprocessor data processing
                    //coprocessor register transfers
                    return -1;
                }else{
                    // software interrupt
                    return INTERRUPTION ;
                }
        }
    }else{ // les conditions ne sont pas verifier
        return INTERRUPTION ;
    }
    return -1 ;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Veuillez fournir un nombre hexadécimal en argument.\n");
        return 1;
    }

    // Convertir le nombre hexadécimal en décimal
    char *endptr;
    uint32_t decimalValue = strtol(argv[1], &endptr, 16);

    if (*endptr != '\0') {
        printf("Erreur de conversion du nombre hexadécimal.\n");
        return 1;
    }

    printf("Valeur décimale : %u\n", decimalValue);
    
    // Afficher la valeur binaire
    printf("Valeur binaire   : ");
    for (int i = sizeof(decimalValue) * 8 - 1; i >= 0; --i) {
        printf("%d", (decimalValue >> i) & 1);
    }
    printf("\n");

    // Utiliser la valeur décimale dans type_instructions
    printf("Type d'instruction : %d\n", type_instructions(decimalValue));

    return 0;
}

