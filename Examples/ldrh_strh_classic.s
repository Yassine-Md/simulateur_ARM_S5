
.global main
.section .data



.text
main:      
    mov r0, #0x12
    mov r1, #0
    mov r1, #0x2000
    strh r0, [r1]
    ldrh r2, [r1]

