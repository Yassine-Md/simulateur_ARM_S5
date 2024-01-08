.global main
.section .data



.text
main:      
    mov r1, #8
    ldr r3, =data_value   // Charge l'adresse de data_value dans r3
    str r1, [r3, r1]       // Stocke la valeur de r1 à l'adresse contenue dans r3 + r1

    add r5, r1, #0x30      // Ajoute 0x30 à la valeur de r1
    ldr r2, [r5]           // Charge la valeur stockée à l'adresse calculée dans r2

.data
data_value: .word 0x30
