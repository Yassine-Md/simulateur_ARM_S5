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
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "util.h"



memory memory_create(size_t size) {
    memory mem = (memory)malloc(sizeof(struct memory_data));
    if (mem != NULL) {
        mem->capacity = size;
        mem->data = (uint8_t *)malloc(size);
        if (mem->data == NULL) {
            free(mem);
            return NULL;
        }
    }
    return mem;
}

size_t memory_get_size(memory mem) {
    if (mem != NULL){
        return mem->capacity;
    }
    return  0;
}

void memory_destroy(memory mem) {
    if (mem != NULL) {
        free(mem->data);
        free(mem);
    }
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if (mem == NULL || address >= mem->capacity )
        return -1;

    *value = mem->data[address];
    return 0;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value, uint8_t be) {
    if (mem == NULL || address >= mem->capacity - 1)
        return -1;

    if (!be) {
        *value = mem->data[address] | (mem->data[address + 1] << 8);
    } else {
        *value = mem->data[address + 1] | (mem->data[address] << 8) ;
    }

    return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be) {
    printf("memory_read_word mem vaut : %p \n and the address : %d \n", mem , address);
    if (mem == NULL || address  >= mem->capacity-3)
        return -1;

    if (!be) {
        *value = mem->data[address] | (mem->data[address + 1] << 8) |
                 (mem->data[address + 2] << 16) | (mem->data[address + 3] << 24);
    } else {
        *value = (mem->data[address]<<24) | (mem->data[address + 1] << 16) |
                 (mem->data[address + 2] << 8) | mem->data[address + 3] ;
    }

    return 0;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if (mem == NULL || address >= mem->capacity)
        return -1;

    mem->data[address] = value;
    return 0;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value, uint8_t be) {
    if (mem == NULL || address + 1 >= mem->capacity)
        return -1;

    if (!be) {
        mem->data[address] = value & 0xFF;
        mem->data[address + 1] = (value >> 8) & 0xFF;
    } else {
        mem->data[address] = (value >> 8) & 0xFF;
        mem->data[address + 1] = value & 0xFF;
    }

    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value, uint8_t be) {
    if (mem == NULL || address >= mem->capacity-3)
        return -1;

    if (!be) {
        mem->data[address] = value & 0xFF;
        mem->data[address + 1] = (value >> 8) & 0xFF;
        mem->data[address + 2] = (value >> 16) & 0xFF;
        mem->data[address + 3] = (value >> 24) & 0xFF;
    } else {
        mem->data[address] = (value >> 24) & 0xFF;
        mem->data[address + 1] = (value >> 16) & 0xFF;
        mem->data[address + 2] = (value >> 8) & 0xFF;
        mem->data[address + 3] = value & 0xFF;
    }

    return 0;
}