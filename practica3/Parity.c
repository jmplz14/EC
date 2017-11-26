//for((i=0;i <11; i++)); do echo$i;./parity;done | pr-11 -l 20 -w 80
//  segÃºn la versiÃ³n de gcc y opciones de optimizaciÃ³n usadas, tal vez haga falta
//  usar gcc â€“fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval
#define TEST 0 
#define COPY_PASTE_CALC 1
#if ! TEXT 
	#define NBITS 20
	#define SIZE (1<<NBITS)
	unsigned int lista[SIZE];
	#define RESULT (1<<NBITS-1)
#else
/************************************ /
	#define SIZE 4
	unsigned int lista[SIZE]={0X80000000, 0X00100000, 0X00000800, 0X00000001}
	#define RESULT 4
/************************************ /	
	#define SIZE 8
	unsigned int lista[SIZE]={0X7FFFFFFF, 0XFFEFFFFF, 0XFFFFF7FF, 0XFFFFFFFE, 0X01000024, 0X00356700, 0X8900AC00, 0X00BC00EF}
	#define RESULT 8
/************************************/
	#define SIZE 8
	unsigned int lista[SIZE]={0x0 , 0x10204080, 0x3590AC06, 0X70B0D0E0, 0XFFFFFFFF, 0X12345678, 0X9ABCDEF0, 0XCAFEBEEF}
	#define RESULT 2
/***********************************/
#endif

int resultado=0;
int parity2(unsigned * array, int len){
	int i;
	unsigned x;
	int val,result=0;
	for  (i=0; i < len ; i++){
		x = array[i];
		val = 0;
		do{
			val ^= x & 0x1;
			x>>=1;
		}while(x);
		result += val;

	}
	return result;

}
int parity3(unsigned* array, int len) {

	int val = 0;
	int i;
	unsigned x;
	int result = 0;
	for (i = 0; i < len; i++) {
		x = array[i];
		while (x) {
			val ^= x;
			x >>= 1;
		}
		//Nos ahorramos las mascaras en todas las pasadas del while
		result += val & 0x1;
	}
	return result;

}
int parity4(unsigned* array, int len) {

	int val;
	int i;
	unsigned x;
	int result = 0;

	for (i = 0; i < len; i++) {
		x = array[i];
		val = 0;
		asm(
			"ini3:	\n\t"
			"xor %[x], %[v]	\n\t"
			"shr $1, %[x]	\n\t"
			"test %[x], %[x] \n\t"
			"jnz ini3	\n\t"
			: [v]"+r"(val)
			: [x]"r"(x)
		);
		result += val & 0x1;
	}
	return result;

}
int parity5(unsigned* array, int len) {

	int i, k;
	int result = 0;
	unsigned x;
	for (i = 0; i < len; i++) {
		x = array[i];
		for (k = 16; k == 1; k /= 2)
			x ^= x >> k;
		result += (x & 0x01);
	}
	return result;

}
int parity6(unsigned* array, int len) {
	int j;
	unsigned entero = 0;


	int resultado = 0;

	for (j = 0; j < len; j++)
	{
		entero = array[j]; 
		asm(
			"mov	%[x], 	%%edx		\n\t"
			"shr	$16,	%%edx		\n\t"
			"xor	%[x],	%%edx		\n\t"
			"xor	%%dh,	%%dl		\n\t"
			"setpo  %%dl				\n\t"
			"movzx	%%dl,	%[x]		\n\t"
			: [x] "+r" (entero) // input
			:
			: "edx"//Clobber
		);
		resultado += entero;
	}
	return resultado;
}
int parity1(unsigned * array, int len){
	int i,j; 
	unsigned x;
	int val,result = 0;

	for(i=0; i < len; i++){
		x = array[i];
		val = 0;
		for(j=0; j < 8*sizeof(unsigned); j++){
			val ^= x & 0x1;
			x >>=1;
		}
		resultado += val;
	}
	return result;
}



void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
#if COPY_PASTE_CALC
	printf("%ld" "\n", tv_usecs);
#else
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
#endif
}

int main()
{
#if ! TEST
    int i;			// inicializar array
    for (i=0; i<SIZE; i++)	// se queda en cache
	 lista[i]=i;
#endif
    crono(parity1, "parity1 (lenguaje c -	for)");
    crono(parity2, "parity2 (lenguaje c -	while)");    
    crono(parity3, "parity2 (lenguaje c -	mask final)");
    crono(parity4, "parity2 (lenguaje c -	cuerpo while)");
    crono(parity5, "parity2 (lenguaje c -	32,16...1b)");
    crono(parity6, "parity2 (lenguaje c -	)");

#if !COPY_PASTE_CALC
    //printf("N*(N+1)/2 = %d\n", (SIZE-1)*(SIZE/2)); /*OF*/
	printf("calculado = %d\n", RESULT); /*OF*/
#endif
    exit(0);
}

