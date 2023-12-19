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
#ifndef __REGISTERS_H__
#define __REGISTERS_H__
#include <stdint.h>
#include <stdio.h>

typedef struct registers_data *registers;

// Fonction : registers_create
// Modifie : Alloue et initialise une structure de registres. Initialise les registres généraux, le CPSR, le mode par défaut (SVC) et les SPSR.
// Return : Pointeur vers la structure de registres nouvellement créée.
registers registers_create();


// Fonction : registers_destroy
// Modifie : Libère la mémoire allouée pour la structure de registres.
// Return : Aucun.
void registers_destroy(registers r);


// Fonction : registers_get_mode
// Modifie : Aucun.
// Return : Mode actuel stocké dans la structure de registres.
uint8_t registers_get_mode(registers r);


// Fonction : registers_mode_has_spsr
// Modifie : Aucun.
// Return : 1 si le mode spécifié a un registre SPSR associé, 0 sinon.
int registers_current_mode_has_spsr(registers r);


// Fonction : registers_in_a_privileged_mode
// Modifie : Aucun.
// Return : 1 si la structure des registres est actuellement dans un mode privilégié, 0 sinon : SYS et USR. 
int registers_in_a_privileged_mode(registers r);


// Fonction : registers_read
// Modifie : Aucun.
// Return : La valeur du registre général spécifié dans le mode spécifié. UINT32_MAX si une erreur se produit.
uint32_t registers_read(registers r, uint8_t reg, uint8_t mode);


// Fonction : registers_read_cpsr
// Modifie : Aucun.
// Return : La valeur du registre CPSR dans la structure de registres. UINT32_MAX si une erreur se produit.
uint32_t registers_read_cpsr(registers r);


// Fonction : registers_read_spsr
// Modifie : Aucun.
// Return : La valeur du registre SPSR associé au mode spécifié. UINT32_MAX si une erreur se produit.
uint32_t registers_read_spsr(registers r, uint8_t mode);


// Fonction : registers_write
// Modifie : Modifie la valeur du registre général spécifié dans le mode spécifié.
// Return : Aucun.
void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value);


// Fonction : registers_write_cpsr
// Modifie : Modifie la valeur du registre CPSR dans la structure de registres. Met à jour également le mode actuel.
// Return : Aucun.
void registers_write_cpsr(registers r, uint32_t value);


// Fonction : registers_write_spsr
// Modifie : Modifie la valeur du registre SPSR associé au mode spécifié.
// Return : Aucun.
void registers_write_spsr(registers r, uint8_t mode, uint32_t value);

#endif
