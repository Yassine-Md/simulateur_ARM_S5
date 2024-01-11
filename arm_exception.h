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
#ifndef __ARM_EXCEPTION_H__
#define __ARM_EXCEPTION_H__
#include <stdint.h>
#include "arm_core.h"

int arm_exception(arm_core p, unsigned char exception);

// Configure le registre CPSR pour la gestion des exceptions, sauvegarde l'état actuel dans le registre SPSR .
void set_exception_cpsr(arm_core p, uint8_t mode , uint8_t bit6 ) ;


/*
Gere une exception d'abord de données en sauvegardant l'adresse de l'instruction avortee, 
configurant le CPSR, puis activant le gestionnaire a l'adresse 0x0000000C
*/
void data_abort(arm_core p) ;

/*
Gere une exception d'interruption en sauvegardant l'adresse de la prochaine instruction, 
configurant le CPSR, puis activant le gestionnaire a l'adresse 0x00000018
*/
void unterrupt(arm_core p) ;

/*
Gere une exception d'instruction indéfinie en sauvegardant l'adresse de l'instruction suivante, 
configurant le CPSR, puis activant le gestionnaire a l'adresse 0x00000004
*/
void undefined_unstruction(arm_core p) ;

/*
Gere une exception d'abord de lecture en sauvegardant l'adresse de l'instruction avortee, 
configurant le CPSR, puis activant le gestionnaire a l'adresse 0x0000000C
*/
void fetch_abort(arm_core p) ;

/*
Gere une exception de demande d'interruption rapide en sauvegardant l'adresse de la prochaine instruction, 
configurant le CPSR, puis activant le gestionnaire a l'adresse 0x0000001C
*/
void fast_interrupt_request(arm_core p) ;

#endif
