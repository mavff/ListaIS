#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#define TAM_MATRIZ 3
typedef struct{
    int var_i;
    float A[TAM_MATRIZ][TAM_MATRIZ];
    float X_K[TAM_MATRIZ];
    float B[TAM_MATRIZ];
    float Solucao[TAM_MATRIZ];
} SEL;//Sistema de Equacoes Lineares
void Metodo_jacobi(SEL*);
pthread_barrier_t barrier;//uso de barreira 
int main(){
    SEL *equacao = malloc(sizeof(SEL)*TAM_MATRIZ);
    pthread_t variavel_equacao[TAM_MATRIZ];
    pthread_barrier_init(&barrier, NULL, TAM_MATRIZ);
    int valores_A[TAM_MATRIZ][TAM_MATRIZ]={4, 3, -5,
                                             -2, 3, 4,
                                             3, -4, 3};
    int valores_X[TAM_MATRIZ] = {0, 0, 0};
    int valores_B[TAM_MATRIZ] = {10, 13, -6};
    for (int i = 0; i < TAM_MATRIZ; i++) {//preencher o vetor ,pois c fede
        
        for (int j = 0; j < TAM_MATRIZ; j++) {
            equacao[i].A[i][j] = valores_A[i][j];
        }
        equacao[i].X_K[i] = valores_X[i];
        equacao[i].B[i] = valores_B[i];
    }
    for(int i = 0; i < TAM_MATRIZ; i++) {//criar a thread e puxar o metodo
        equacao[i].var_i=i;
        pthread_create(&variavel_equacao[i],NULL,Metodo_jacobi,&equacao[i]);  
    }
    for (int i = 0; i < TAM_MATRIZ; ++i) {
        pthread_join(variavel_equacao[i], NULL);
    }
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
}
void Metodo_jacobi(SEL* sistema){
    int p=15,k=0;
    int i=sistema->var_i;//variavel i=1,2,3,4...n que estamos trabalhando
    float* x_k_mais_1=&sistema->Solucao[i];
    float b_i=sistema->B[i];//termo Bi da matriz;
    float a_i_i=sistema->A[i][i];//termo Aii da matriz
    float somatorio=0;
    while(k<p){
        for(int j=0;j<TAM_MATRIZ;j++){//fazer o somatorio
            if(j!=i){//i!=j
                somatorio+=(sistema->A[i][j])*(sistema->X_K[j]);
            }
        }
        *x_k_mais_1=(b_i-somatorio)/a_i_i;
        k++;
        somatorio=0;
        pthread_barrier_wait(&barrier);

    }
    printf("Solucao da variavel X_%d : %.6f\n",i+1,*x_k_mais_1);
}