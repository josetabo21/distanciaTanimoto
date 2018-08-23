#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define MAX_TAM 16
#define D_N 12422

typedef struct{
    char identif[13];
    int compContado[MAX_TAM];

}compuesto;

float coefTanimoto(int *i, int *j){
    int Na, Nb, Nc, aa, ab, ac;
    Na= Nb= Nc= aa= ab= ac =0;
    int cont=0;
    while(cont<MAX_TAM){
        if(*i>0 || *j>0){
            if(cont==0){
                if(*i>0 && *j>0){
                    aa= ab= ac =1;
                }else if(*i>0){
                    aa=1;
                }else{
                    ab=1;
                }
            }else{
                if(*i > *j){
                    Nc = Nc + *j;
                }else{
                    Nc = Nc + *i;
                }
                Na = Na + *i;
                Nb = Nb + *j;
            }
        }
        cont++;
        i++;
        j++;
    }
    Na = Na + aa;
    Nb = Nb + ab;
    Nc = Nc + ac;

    return (float)Nc / (float)(Na + Nb - Nc);
}

int *contarCh(char *formula){

	static int compContd[MAX_TAM];//ojo
	int i=0;
	for(;i<16;i++){
        compContd[i]=0;
	}

	int j=0,patron[]={'@','c','C','F','H','I','N','O','P','S','B','l','n','o','r','s'};

    while(*formula != '\0'){
            //printf("hola");
        for(j=0;j<16;j++){
            if(*formula==patron[j]){
                compContd[j]++;
                break;
            }
        }
        formula++;
    }
   // printf("\n%d",compContd[0]);
    //compContd[j]=-1;
	return compContd;
}

int main()
{
    FILE *archivoEntrada, *archivoSalida;

    char formula[200];
    static char identf[]="ccccccccccccc";
    char c;
    int pos, *p, i, x=0, y=0, cont;
    float coef;

    i=0;coef=0.0;

    clock_t start=clock();
    archivoEntrada = fopen("ZINC_chemicals.tsv", "r");

    compuesto comp;
    compuesto listaComp[D_N];
    while(1){

        c = fgetc(archivoEntrada);
         //printf("hola");
        //col 1
        if(c=='c'){
            //c = fgetc(archivoEntrada);
            pos=0;
            //col 2

            do{
                identf[pos]=c;
                pos++;
                c = fgetc(archivoEntrada);
            }while(c != '\t'); //no llegue a col 3

            identf[pos] = '\0';
            //col 3
            do{
                c = fgetc(archivoEntrada);
            }while(c != '\t');
            //col 4
            pos=0;
            do{
                c = fgetc(archivoEntrada);
                formula[pos] = c;
                pos++;
            }while(c != '\n');
            formula[pos]='\0';

            //cuenta letras y guarda todo en estruct de Compuesto
            p = contarCh(formula);
            pos=0;
           // printf("\n%d",*p);
            while(pos<MAX_TAM){
                comp.compContado[pos] = *p;
                p++;
                pos++;
            }
            strcpy(comp.identif, identf);
            //comp = (compuesto *) realloc(comp,(i+1)*sizeof(compuesto))

            listaComp[i++]=comp;
        }
        if(feof(archivoEntrada)){
            break;
        }
    }
    fclose(archivoEntrada);

    /*for(fil=0;fil<D_N;fil++){
            printf("\n%s\t%d",listaComp[fil].identif,listaComp[fil].compContado[0]);

    }*/
    archivoSalida = fopen("CoeficientesDT.txt","w");
    printf("Tiempo A-C: %f",(double)(clock()-start)/CLOCKS_PER_SEC);

    start=clock();
    omp_set_num_threads(omp_get_max_threads());

    #pragma omp parallel for ordered //schedule(static,1)
    for(x=0; x<D_N; x++){
        for(y=x+1; y<D_N; y++){
            coef = coefTanimoto(listaComp[x].compContado, listaComp[y].compContado);
            #pragma omp ordered
            //printf("%s\t%s\t%.2f\n",listaComp[x].identif, listaComp[y].identif, coef);
            fprintf(archivoSalida, "%s\t%s\t%.2f\n",listaComp[x].identif, listaComp[y].identif, coef);
        }
    }
    fclose(archivoSalida);
    printf("\n\nTiempo Ejecucion en Paralelizacion: %f",(double)(clock()-start)/CLOCKS_PER_SEC);

    return 0;

}

