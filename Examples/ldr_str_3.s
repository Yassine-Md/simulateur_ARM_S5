.global main
.section .data

.text
main:
    // Charger l'adresse 0x1800 dans le registre r4
    ldr r4, =0x1800

    // Utiliser le registre r4 avec strh et ldrh
    mov r2, #0x100
    str r2, [r4]

    ldr r3, [r4]

    swi 0x123456
