/*
Um quadrado mágico é uma matriz quadrada 
em que todas as linhas, colunas e diagonais têm a mesma soma. 
Faça um programa usando pthreads para verificar 
se a matriz quadrada é um quadrado mágico.
Crie uma constante para que o programa assuma N 
para a verificação do quadrado mágico. 
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define TAM 5
typedef struct{
    int **ptr;
    int cont[TAM];
    bool resultado;
}Contador;//apenas para salvas os resultados de cada vetor;
void Somar_linhas(Contador*);
void Somar_colunas(Contador*);
void Somar_diagonal(Contador*);
pthread_barrier_t barrier;
int main(){
    int **matriz;
    Contador soma_linha,soma_coluna,soma_diag;
    matriz=(int**) malloc(sizeof(int*)*TAM);
    //INICIALIZAR UMA MATRIZ QUADRADA DE TESTE
    for(int i=0;i<TAM;i++){
        matriz[i]=(int*) malloc(sizeof(int)*TAM);
        soma_coluna.cont[i]=0;
        soma_diag.cont[i]=0;
        soma_linha.cont[i]=0;
        for(int j=0;j<TAM;j++){
            matriz[i][j]=1;
        }
    }
    ////
    soma_coluna.ptr=matriz;
    soma_diag.ptr=matriz;
    soma_linha.ptr=matriz;
    pthread_t linha,coluna,diagonal;//3 THREADS
    pthread_barrier_init(&barrier, NULL, 3);//barreira para as 3 threads
    //
    pthread_create(&linha, NULL,Somar_linhas , &soma_linha);
    pthread_create(&coluna, NULL,Somar_colunas , &soma_coluna);
    pthread_create(&diagonal, NULL,Somar_diagonal , &soma_diag);
    //apenas para juntar as 3 threads ao final do metodo/funcao executado por cada uma
    pthread_join(linha, NULL);
    pthread_join(coluna, NULL);
    pthread_join(diagonal, NULL);
    //
    if(soma_linha.resultado && soma_coluna.resultado && soma_diag.resultado){
        printf("Temos um quadrado magico!!\n");
    }else{
        printf("Nao temos um quadrado magico :(...\n");
    }
    //
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
    //
    
}
void Somar_linhas(Contador* calculo){
    int** temp=calculo->ptr;
    calculo->resultado=true;
    bool erro=true;
    for(int i=0;i<TAM && erro;i++){
        for(int j=0;j<TAM;j++){
            calculo->cont[i]+=temp[i][j];
        }
        if(i>0&&(calculo->cont[i]!=calculo->cont[i-1])){
           calculo->resultado=false;
           erro=false;
           break;
        }
    }
    pthread_barrier_wait(&barrier);//esperar as outras threas ficarem prontas
}
void Somar_colunas(Contador* calculo){
    int** temp=calculo->ptr;
    calculo->resultado=true;
    bool erro=true;
    for(int i=0;i<TAM && erro;i++){
        for(int j=0;j<TAM;j++){
            calculo->cont[i]+=temp[j][i];
        }
        if(i>0&&(calculo->cont[i]!=calculo->cont[i-1])){
           calculo->resultado=false;
           erro=false;
           break;
        }
    }
    pthread_barrier_wait(&barrier);//esperar as outras threas ficarem prontas
}
void Somar_diagonal(Contador* calculo){
    int** temp=calculo->ptr;
    calculo->resultado=false;
    if(TAM>1){
        for (int i = 0; i < TAM; i++) {
            calculo->cont[0] += temp[i][i];
            calculo->cont[1] += temp[i][TAM - i - 1];
        }
        if(calculo->cont[0]==calculo->cont[1]){
            calculo->resultado=true;
        }
    }else{
        calculo->resultado=true;
    }
    pthread_barrier_wait(&barrier);//esperar as outras threas ficarem prontas
}