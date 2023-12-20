#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define TAM 15
typedef struct{
    int *array;
    int inicio;
    int fim;
}Limites_array;
void merge(int* arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];

    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0; 
    j = 0; 
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
void Sort(int* arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        Sort(arr, l, m);
        Sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}
void Merge_Sort(Limites_array* parte){
    Sort(parte->array,parte->inicio,parte->fim);
}
int main(){
    Limites_array vetor_esq,vetor_dir;
    int vetor[TAM]={2,5,1,90,23,45,28,56,12,11,76,231,47,245,223};
    vetor_esq.inicio=0;
    int meio=TAM/2;

    vetor_esq.array=vetor;
    vetor_esq.inicio=0;
    vetor_esq.fim=meio;
    vetor_dir.array=vetor;
    vetor_dir.inicio=meio;
    vetor_dir.fim=TAM;
    //
    pthread_t esquerda,direita;//merge sorte na metade da esquerda e na metade da direita
    pthread_create(&esquerda,NULL,Merge_Sort,&vetor_esq);
    pthread_create(&direita,NULL,Merge_Sort,&vetor_dir);
    //
    pthread_join(esquerda, NULL);
    pthread_join(direita, NULL);
    //
    merge(vetor, 0, meio - 1, TAM - 1);//dar um ultimo merge nas das metades ja ordenadas
    printf("[ ");
    for(int i=0;i<TAM;i++){
        printf("%d ",vetor[i]);
    }
    printf(" ]");
}
