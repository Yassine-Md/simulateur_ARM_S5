.text
main:
    mov r0, #0x1800
    mov r2, #100

    // Stocke la valeur à l'adresse spécifique dans le segment de données
    str r2, [r0 , #200]

    // Charge la valeur depuis l'adresse spécifique dans le segment de données
    ldr r3, [r0 , #200]

    swi 0x123456

