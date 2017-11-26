.section .data
.macro linea 
#	.int 1,1,1,1
#	.int 2,2,2,2
	.int 1,2,3,4
#	.int -1,-1,-1,-1
#	.int 0x08000000,0x08000000,0x08000000,0x08000000
#	.int 0x10000000,0x20000000,0x40000000,0x80000000
.endm
lista: .irpc i,12345678
	linea
	.endr
longitud:
	.int (.-lista)/4
resultado:
	.quad 0
formato:
	.ascii "suma = %llu = %llx hex\n\0"	

.section .text
main: .global main
	movl $lista, %ebx
	movl longitud, %ecx
	call suma
	mov %eax, resultado
	mov %edx, resultado + 4
	push resultado + 4
	push resultado
	push resultado + 4
	push resultado 
	push $formato
	call printf
	add $20, %esp

	mov $1, %eax
	mov $0, %ebx
	int $0x80
suma: 
	
	movl $0, %esi
	movl $0, %eax
	movl $0, %edx

bucle:
	add (%ebx,%esi,4), %eax
	jnc noacarreo
	inc %edx
noacarreo:
	inc %esi
	cmp %esi, %ecx
	jne bucle
	ret



