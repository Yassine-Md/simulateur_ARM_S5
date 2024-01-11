.global main
.section .data



.text
main:      
    mov r0, #0x12
    mov r1, #0x2000
    str r0, [r1]
    ldr r2, [r1]

.data
a : .word 0x4
b : .word 0x4
c : .word 0x4
data_value: .word 0x30
