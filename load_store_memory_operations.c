#include "arm_exception.h"
#include "arm_constants.h"
#include "load_store_memory_operations.h"
#include "util.h"
#include "debug.h"


int handle_memory_operation(arm_core p, uint32_t ins, uint32_t write_back) {
    uint32_t offset, Rn, U, B, load_store, Rd;
    // Choose the correct offset based on the instruction type
    if (is_register_offset(ins)) {
        uint32_t Rm;
        initialize_register_variables(p, ins, &Rd, &Rm, &Rn, &U, &load_store, &B);
        offset = arm_read_register(p, Rm);
    }else if (is_scaled_register_offset(ins)) {
        uint32_t shift, shift_imm, Rm;
        uint32_t C_flag = get_bit(arm_read_cpsr(p), 29);
        initialize_scaled_register_variables(p, ins, &shift, &shift_imm, &Rd, &Rm, &Rn, &U, &load_store, &B);
        offset = calculate_address_scaled_register(p, Rn, arm_read_register(p, Rm), shift, shift_imm, U, C_flag);
    }else if (is_immediate_offset(ins)) {
        uint32_t offset_12;
        initialize_immediate_variables(p, ins, &Rd, &Rn , &offset_12, &U, &load_store, &B);
        offset = offset_12;
    } else {
        // Handle other offset types if needed in the future
        return 1;  // Unsupported offset type
    }

    uint32_t address = calculate_address(p, Rn, offset, U);
    if (write_back) {
        arm_write_register(p, Rn, address);
    }

    return process_memory_access(p, address, load_store, B, Rd);
}


// fonction pour faire les accees memoire et recuperer les valeurs soit pour effectuer le ldr soit le str
int process_memory_access(arm_core p, uint32_t address, int load_store, int B, uint32_t Rd) {
    if (load_store == 0) { // Store
        if (B == 0) { // Word
            return arm_write_word(p, address, arm_read_register(p, Rd));
        } else { // Byte
            return arm_write_byte(p, address, (uint8_t)arm_read_register(p, Rd));
        }
    } else { // Load
        if (B == 0) { // Word
            uint32_t chargedValue_word;
            arm_read_word(p, address, &chargedValue_word);
            return arm_write_register(p, Rd, chargedValue_word);
        } else { // Byte
            uint8_t chargedValue_byte;
            arm_read_byte(p, address, &chargedValue_byte);
            return arm_write_register(p, Rd, chargedValue_byte);
        }
    }
}


int process_memory_access_half(arm_core p, uint32_t address, uint32_t load_store, uint32_t Rd) {
    uint16_t valueToWriteOrRead;

    if (load_store == 0) { // store
        valueToWriteOrRead = (uint16_t)arm_read_register(p, Rd);
        return arm_write_half(p, address, valueToWriteOrRead);
    } else { // ldr
        arm_read_half(p, address, &valueToWriteOrRead);
        return arm_write_register(p, Rd, valueToWriteOrRead);
    }
}


// Common initialization function
void initialize_common_variables(arm_core p, uint32_t ins,
                          uint32_t *Rd, uint32_t *Rn,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    *Rd = get_bits(ins, 15, 12);
    *Rn = get_bits(ins, 19, 16);
    *U = get_bit(ins, 23);
    *load_store = get_bit(ins, 20);
    *B = get_bit(ins, 22);
}


void initialize_immediate_variables(arm_core p, uint32_t ins,
                          uint32_t *Rd, uint32_t *Rn, uint32_t *offset_12,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    initialize_common_variables(p,ins,Rd,Rn,U,load_store,B);
    *offset_12 = get_bits(ins , 11 , 0);
}

void initialize_register_variables(arm_core p, uint32_t ins,
                          uint32_t* Rd, uint32_t* Rm, uint32_t* Rn,
                          uint32_t* U, uint32_t *load_store, uint32_t *B) {
    initialize_common_variables(p,ins,Rd,Rn,U,load_store,B);
    *Rm = get_bits(ins, 3, 0);
}

void initialize_scaled_register_variables(arm_core p, uint32_t ins,
                          uint32_t *shift, uint32_t *shift_imm,
                          uint32_t *Rd, uint32_t *Rm, uint32_t *Rn,
                          uint32_t *U, uint32_t *load_store, uint32_t *B) {
    initialize_register_variables(p,ins,Rd,Rm,Rn,U,load_store,B);
    *shift = get_bits(ins, 6, 5);
    *shift_imm = get_bits(ins, 11, 7);
}