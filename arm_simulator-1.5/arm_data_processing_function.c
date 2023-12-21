#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"
#include "registers.c"
#include "arm_core.c"

// Fonction pour mettre à jour les flags du CPSR
void mise_a_jour_flags_cpsr(arm_core p, uint32_t res, uint32_t op_gauche)
{
    uint8_t Nflag = get_bit(res, 31);
    uint8_t Zflag = (res == 0);
    uint8_t Cflag = (res > op_gauche);
    uint8_t Vflag = (get_bit(op_gauche, 31) && get_bit(res, 31));
    uint8_t aux = ((Nflag << 3) | (Zflag << 2) | (Cflag << 1) | Vflag);
    arm_write_cpsr(p, (arm_read_cpsr(p) & 0x0FFFFFFF) | (aux << 28));
}

void mise_a_jour_N_Z_cpsr(arm_core p, uint32_t res)
{
    uint8_t Nflag = get_bit(res, 31);
    uint8_t Zflag = (res == 0);

    // Ne pas affecter les flags C et V

    uint8_t aux = ((Nflag << 3) | (Zflag << 2));
    arm_write_cpsr(p, (arm_read_cpsr(p) & 0x0FFFFFFF) | (aux << 28));
}

uint64_t adc(arm_core p, uint32_t ins, uint32_t op_gauche, uint32_t op_droite, uint64_t res, int carry_out)
{
    res = op_gauche + op_droite + carry_out;
    if (get_bit(ins, 20) == 1)
    { // mise a jour du cpsr
        mise_a_jour_flags_cpsr(p, res, op_gauche);
    }
    return res;
}

uint64_t sbc(arm_core p, uint32_t ins, uint32_t op_gauche, uint32_t op_droite, uint64_t res, int carry_out)
{
    uint32_t op_droite_with_carry = op_droite + carry_out;
    res = (uint64_t)op_gauche - op_droite_with_carry;

    if (get_bit(ins, 20))
    { // mise ajour du cpsr
        mise_a_jour_flags_cpsr(p, res, op_gauche);
    }
    return res;
}

uint32_t eor(arm_core p, uint32_t ins, uint32_t op_gauche, uint32_t op_droite, uint32_t res)
{
    res = op_gauche ^ op_droite;
    if (get_bit(ins, 20))
    {
        mise_a_jour_flags_cpsr(p, res, op_gauche);
    }
    return res;
}

void teq(arm_core p, uint32_t op_gauche, uint32_t op_droite)
{
    uint32_t res = op_gauche ^ op_droite;
    mise_a_jour_N_Z_cpsr(p, res);
}

void cmp(arm_core p, uint32_t op_gauche, uint32_t op_droite)
{
    uint32_t res = op_gauche - op_droite;
    mise_a_jour_flags_cpsr(p, res, op_gauche);
}

void cmn(arm_core p, uint32_t op_gauche, uint32_t op_droite)
{
    uint32_t res = op_gauche + op_droite;
    mise_a_jour_flags_cpsr(p, res, op_gauche);
}

void rsc(arm_core p, uint32_t ins, uint32_t op_gauche, uint32_t op_droite, int carry_in)
{
    uint32_t res = op_droite - op_gauche - (1 - carry_in);
    // mise a jour particuliere des flags
    if (get_bit(ins, 20))
    {
        uint8_t Nflag = get_bit(res, 31);
        uint8_t Zflag = (res == 0);
        uint8_t Cflag = (op_droite > op_gauche + (1 - carry_in));
        uint8_t Vflag = ((op_droite >> 31) != (op_gauche >> 31)) && (Nflag != (op_gauche >> 31));
        uint8_t aux = ((Nflag << 3) | (Zflag << 2) | (Cflag << 1) | Vflag);
        arm_write_cpsr(p, (arm_read_cpsr(p) & 0x0FFFFFFF) | (aux << 28));
    }
}

uint32_t bic(arm_core p, uint32_t ins, uint32_t op_gauche, uint32_t op_droite)
{
    uint32_t res = op_gauche & ~(op_droite);
    if (get_bit(ins, 20))
    {
        uint8_t Nflag = get_bit(res, 31);
        uint8_t Zflag = (res == 0);
        mise_a_jour_N_Z_cpsr(p, res);
    }
    return res;
}