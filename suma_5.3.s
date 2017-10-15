.section .data
.macro linea 
#	.int 1,1,1,1
#	.int 2,2,2,2
#	.int 1,2,-3,-4
	.int 1,-2,1,-2
#	.int 0x08000000,0x08000000,0x08000000,0x08000000
#	.int 0x10000000,0x20000000,0x40000000,0x80000000
.endm
lista: .irpc i,12345678
	linea
	.endr
longitud:
	.int (.-lista)/4
media:
	.int 0
resto:
	.int 0
formato:
	.ascii "media = %lu resto = %lu \n\0"	

.section .text
main: .global main
	movl $lista, %ebx
	movl longitud, %ecx
	call suma
	mov %eax, media
	push resto
	push media
	push $formato
	call printf
	add $12, %esp

	mov $1, %eax
	mov $0, %ebx
	int $0x80
suma: 
	
	movl $0, %ebp
	movl $0, %edi
	movl $0, %edx

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
	idivl %ecx
	mov %edx, resto
	ret



