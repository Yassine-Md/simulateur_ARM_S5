.global main
.text


main:
	mov r2,#8
	bl end
	mov r2,#1

	
hello:	mov r2,#9

end:	mov r2,#100
	swi 0x123456
	
