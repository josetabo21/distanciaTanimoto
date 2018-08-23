#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define tamanio 32
#define numElementos 12422

typedef struct{
	char identificador[12];
	int contadorCompuestos[numElementos];
}compuestos;

float habllarCoeficienteTanimoto(int *x, int *y){
	int na, nb, nc;
	int a, b, c;
	na=nb=nc=a=b=c=0;
	int contador=0;
	while(contador<tamanio){
		if(*x||*y){
			if(contador==0){
				if(*x>0 && *y>0){
				a=b=c=1;	
				}else if(*x>0){
					a=1;
				}else{
					b=1;
				}
			}else{
				if(*x>*y){
					nc=nc+*y;
				}else{
					nc=nc+*x;
				}
				na=na+*x;
				nb=nb+*y;
			}
		}
		contador++;
		x++;
		y++;
	}
	na=na+a;
	nb=nb+b;
	nc=nc+c;
	return (float) nc/(float)(na+nb-nc)

}
int *cuentaElementos()

