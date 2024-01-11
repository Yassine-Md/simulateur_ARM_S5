.global main
.text
main:
    mov r0, #0xFF000
    mov r4, #1
    mov r5,#2
    mov r6,#3
    ldr r1, =donnee
    stm r1, {r4,r5,r6}
    ldm r1, {r7,r8,r9}
    mov r10 , #5
    swi 0x123456
.data
donnee:
    .word 0x11223344
    .word 0x55667788
    .word 0x55443322
fin: