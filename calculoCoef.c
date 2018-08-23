#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define tamanio 16
#define numElementos 12422

typedef struct{
	char identificador[13];
	int contadorCompuestos[numElementos];
}compuestos;

float hallarCoeficienteTanimoto(int *x, int *y){
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
	return (float) nc/(float)(na+nb-nc);

}
int *cuentaElementos(char *cadena){
	static int contadorCompuestos[tamanio];
	int i=0;
	for(;i<16;i++){
		contadorCompuestos[i]=0;
	}
	int j=0, listaElementos[]={'C','c','N','O','B','F','H','S','o','s','n','r'};
	while(*cadena!='\0'){
		for(j=0;j<16;j++){
			if(*cadena==listaElementos[j]){
				contadorCompuestos[j]++;
				break;
			}
		}
		cadena++;
	}
	return contadorCompuestos;
}
int main(){
	FILE *archivoEntrada, *archivoSalida;
	char cadena[200];
	static char identificador[]="ccccccccccccc";
	char c;
	int posicion, *p, i, x=0,y=0, acumulador;
	float coeficiente;
	i=0;
	coeficiente=0.0;
	
	clock_t start=clock();
	archivoEntrada=fopen("ZINC_chemicals.tsv","r");
	compuestos compue;
	compuestos vectorCompuestos[numElementos];
	while(1){
		c=fgetc(archivoEntrada);
		if(c=='c'){
			posicion =0;
			do{
				identificador[posicion]=c;
				posicion++;
				c=fgetc(archivoEntrada);
			}while(c!='\t');
				identificador[posicion]='\0';
				do{
					c=fgetc(archivoEntrada);
				}
				while(c!='\t');
				posicion=0;
					do{
						c=fgetc(archivoEntrada);
						cadena[posicion]=c;
						posicion++;
					}
					while(c!='\n');
						cadena[posicion]='\0';
						p=cuentaElementos(cadena);
						posicion=0;
					
					while(posicion<tamanio){
						compue.contadorCompuestos[posicion]=*p;
						p++;
						posicion++;
					}
					strcpy(compue.identificador,identificador);
					vectorCompuestos[i++]=compue;
				
				if(feof(archivoEntrada)){
					break;
				}
			
			fclose(archivoEntrada);
		}
	}
	archivoSalida=fopen("SolucionJaccardTanimoto.tsv","w");
	printf("Tiempo A-C: %f",(double)(clock()-start)/CLOCKS_PER_SEC);
	start=clock();
	omp_set_num_threads(omp_get_max_threads());
	#pragma omp parallel for ordered
	for(x=0;x<numElementos;x++){
		for(y=x+1;y<numElementos;y++){
			coeficiente=hallarCoeficienteTanimoto(vectorCompuestos[x].contadorCompuestos,vectorCompuestos[y].contadorCompuestos);
			#pragma omp ordered
			fprintf(archivoSalida,"%s\t%s\t%.2f\n",vectorCompuestos[x].identificador,vectorCompuestos[y].identificador,coeficiente);
		}
	}
	fclose(archivoSalida);
	printf("\n\nTiempo Ejecucion en Paralelizacion: %f",(double)(clock()-start)/CLOCKS_PER_SEC);
	return 0;
	
}

