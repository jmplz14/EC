//for((i=0;i <11; i++)); do echo$i;./popcount;done | pr-11 -l 20 -w 80
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
	#define RESULT (NBITS*(1<<NBITS-1)
#else
/************************************ /
	#define SIZE 4
	unsigned int lista[SIZE]={0X80000000, 0X00100000, 0X00000800, 0X00000001}
	#define RESULT 4
/************************************ /	
	#define SIZE 8
	unsigned int lista[SIZE]={0X7FFFFFFF, 0XFFEFFFFF, 0XFFFFF7FF, 0XFFFFFFFE, 0X01000024, 0X00356700, 0X8900AC00, 0X00BC00EF}
	#define RESULT 156
/************************************/
	#define SIZE 8
	unsigned int lista[SIZE]={0x0 , 0x10204080, 0x3590AC06, 0X70B0D0E0, 0XFFFFFFFF, 0X12345678, 0X9ABCDEF0, 0XCAFEBEEF}
	#define RESULT 116
/***********************************/
#endif

int resultado=0;
int popcount2(unsigned * array, int len){
	int i;
	unsigned x;
	int result=0;
	for  (i=0; i < len ; i++){
		x = array[i];
		do{
			result += x & 0x1;
			x>>=1;
		}while(x);

	}
	return result;

}
int popcount3(unsigned * array, int len){
	int i;
	unsigned x;
	int result=0;
	for  (i=0; i < len ; i++){
		x = array[i];
		asm("\n"
                	"ini3:                             \n\t"
                        "shr $0x1, %[x]        \n\t"
                        "adc $0x0, %[res]        \n\t"
                        "test %[x], %[x]       \n\t"
                        "jnz ini3 "

                        : [res] "+res" (result)
                        : [x] "res" (x) );

	}
	return result;

}
int popcount4(unsigned * array, int len){
	int i, k;
        int result = 0;
        for (i = 0; i < len; i++) {
                int valor = 0;
                unsigned x = array[i];
                for (k = 0; k < 8; k++) {
                        valor += x & 0x01010101;
                        x >>= 1;
                }
                valor += (valor >> 16);
                valor += (valor >> 8);
                result += (valor & 0xff);
        }
        return result;

}

int popcount5(unsigned * array, int len){
	int i;
        int val, result = 0;
        int SSE_mask[] = { 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f };
        int SSE_LUTb[] = { 0x02010100, 0x03020201, 0x03020201, 0x04030302 };

        if (len & 0x3)
                printf("leyendo 128b pero len no múltiplo de 4?\n");
        for (i = 0; i < len; i += 4) {
                asm("movdqu        %[x], %%xmm0 \n\t"
                                "movdqa  %%xmm0, %%xmm1 \n\t"
                                "movdqu    %[m], %%xmm6 \n\t"
                                "psrlw           $4, %%xmm1 \n\t"
                                "pand    %%xmm6, %%xmm0 \n\t"
                                "pand    %%xmm6, %%xmm1 \n\t"

                                "movdqu    %[l], %%xmm2 \n\t"
                                "movdqa  %%xmm2, %%xmm3 \n\t"
                                "pshufb  %%xmm0, %%xmm2 \n\t"
                                "pshufb  %%xmm1, %%xmm3 \n\t"

                                "paddb   %%xmm2, %%xmm3 \n\t"
                                "pxor    %%xmm0, %%xmm0 \n\t"
                                "psadbw  %%xmm0, %%xmm3 \n\t"
                                "movhlps %%xmm3, %%xmm0 \n\t"
                                "paddd   %%xmm3, %%xmm0 \n\t"
                                "movd    %%xmm0, %[val] \n\t"
                                : [val]"=r" (val)
                                : [x] "m" (array[i]),
                                [m] "m" (SSE_mask[0]),
                                [l] "m" (SSE_LUTb[0])
                );
                result += val;
        }
        return result;
}

int popcount1(unsigned * array, int len){
	int i,j; 
	unsigned x;
	int result = 0;

	for(i=0; i < len; i++){
		x = array[i];
		for(j=0; j < 8*sizeof(unsigned); j++){
			result += x & 0x1;
			x >>=1;
		}
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
    crono(popcount1, "popcount1 (lenguaje c -	for)");
    crono(popcount2, "popcount2 (lenguaje c -	while)");
    crono(popcount3, "popcount3 (lenguaje c -	while con examblador)");
    crono(popcount4, "popcount4 (lenguaje c -	version libro)");
    crono(popcount5, "popcount5 (lenguaje c -	Código SSSE3 para fast popcount)");

#if !COPY_PASTE_CALC
    //printf("N*(N+1)/2 = %d\n", (SIZE-1)*(SIZE/2)); /*OF*/
	printf("calculado = %d\n", RESULT); /*OF*/
#endif
    exit(0);
}

