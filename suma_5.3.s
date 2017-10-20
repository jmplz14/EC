.section .data
.macro linea 
	.int -1,-1,-1,-1
#	.int 0,-2,-1,-1
#	.int 1,-2,-1,-1
#	.int 0,-1,-1,-1
#	.int 1,-2,1,-2
#	.int 1,2,-3,-4
#	.int 0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF
#	.int 0x80000000,0x80000000,0x80000000,0x80000000
#	.int 0x04000000,0x04000000,0x04000000,0x04000000
#	.int 0x08000000,0x08000000,0x08000000,0x08000000
#	.int 0xFC000000,0xFC000000,0xFC000000,0xFC000000
#	.int 0xF0000000,0xE0000000,0xE0000000,0xD0000000
#	.int 0xF8000000,0xF8000000,0xF8000000,0xF8000000
#	.int 0xF0000000,0xE0000000,0xE0000000,0xD0000000
.endm
.macro linea0
	.int 0,-2,-1,-1
.endm	
lista: linea0
	.irpc i,1234567#8
		linea
	.endr
longitud:
	.int (.-lista)/4
media: 
	.int 0x89ABCDEF
resto:
	.int 0x01234567
formato:
	.ascii "media = %8d \t resto = %8d \n"	
	.ascii "hexadecimal 0x%08X  \t resto = 0x%08x \n"
.section .text
main: .global main
	movl $lista, %ebx
	movl longitud, %ecx
	call suma
	mov %eax, media
	mov %edx, resto
	push resto
	push media
	push resto
	push media 
	push $formato
	call printf
	add $20, %esp

	mov $1, %eax
	mov $0, %ebx
	int $0x80
suma: 
	
	movl $0, %ebp
	movl $0, %edi
	movl $0, %esi

bucle:
	movl (%ebx,%esi,4), %eax
	cltd
	add %eax, %edi
	adc %edx, %ebp
	inc %esi
	cmp %esi, %ecx
	jne bucle
	mov %edi, %eax
	mov %ebp, %edx
	idiv %ecx
	ret



