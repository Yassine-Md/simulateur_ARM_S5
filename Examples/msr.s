.global _start

.section .text
_start:
    MRS r0, CPSR
    swi 0x123456