.global main
.text


main:
	mov r2,#0x30
	bx r2
	mov r2,#1

	
hello:	mov r2,#9

end:	mov r2,#100
	swi 0x123456
	

