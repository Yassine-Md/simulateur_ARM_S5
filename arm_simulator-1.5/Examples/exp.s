.global _start

.section .data
my_data:
    .word 0x12345678

.section .text
_start:
    ldr r0, =my_data
    ldr r1, [r0]

    mov r7, #4
    mov r0, #1
    ldr r2, =my_data
    mov r3, #4
    swi 0

    mov r7, #1
    mov r0, #0
    swi 0